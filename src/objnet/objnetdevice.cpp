#include "objnetdevice.h"
#include "objnetmaster.h"
#include "stdint.h"

using namespace Objnet;

ObjnetDevice::ObjnetDevice(unsigned char netaddr) :
    mMaster(0L),
    mInfoValidFlags(0),
    mParent(0L),
    mNetAddress(netaddr),
    mPresent(false),
    mConnectionError(false),
    mTimeout(5), mTempTimeout(1),
    mAutoDelete(false),
    mIsLocal(false),
    mBusType(BusUnknown),
    mObjectCount(0)
//    mStateChanged(false),
//    mOrphanCount(0),
//    mChildrenCount(0)
{
    //mTimer.start();
}
//---------------------------------------------------------

void ObjnetDevice::prepareObject(const ObjectInfo::Description &desc)
{
    ObjectInfo *obj = &mObjMap[desc.name];
    obj->mIsDevice = true;
    obj->onValueChanged = EVENT(&ObjnetDevice::onObjectValueChanged);

    unsigned char wt = desc.wType;
    unsigned char rt = desc.rType;
    
    if (obj->mWritePtr && desc.writeSize && desc.wType != obj->mDesc.wType)
    {
        wt = obj->mDesc.wType;
        #ifdef QT_CORE_LIB
        qDebug() << "[ObjnetDevice::prepareObject] write type mismatch!!";
        #endif
    }
    
    if (obj->mReadPtr && desc.readSize && desc.rType != obj->mDesc.rType)
    {
        rt = obj->mDesc.rType;
        #ifdef QT_CORE_LIB
        qDebug() << "[ObjnetDevice::prepareObject] read type mismatch!!";
        #endif
    }

    obj->mDesc = desc;
    obj->mDesc.wType = wt;
    obj->mDesc.rType = rt;
    unsigned char id = desc.id;
    if (id >= mObjects.size())
    {
        int sz = mObjects.size();
        mObjects.resize(id + 1);
        mObjBuffers.resize(id + 1);
        for (int i=0; i<sz; i++)
        {
            ObjectInfo *o = mObjects[i];
            if (o)
            {
                char *ptr = mObjBuffers[i].data();
                if (o->mWritePtr && o->mWritePtr != ptr)
                {
                    if (o->mReadPtr == o->mWritePtr)
                        o->mReadPtr = o->mWritePtr = ptr;
                    else if (!o->mReadPtr)
                        o->mWritePtr = ptr;
                    else
                    {
                        #ifdef Q_OS_LINUX
                        int off = (uintptr_t)o->mReadPtr - (uintptr_t)o->mWritePtr;
                        #else
                        int off = (int)o->mReadPtr - (int)o->mWritePtr;
                        #endif
                        o->mWritePtr = ptr;
                        o->mReadPtr = ptr + off;
                    }
                }
                else if (o->mReadPtr && o->mReadPtr != ptr)
                {
                    o->mReadPtr = ptr;
                }
            }
        }
        for (int i=sz; i<id; i++)
            mObjects[i] = 0L;
    }

    mObjects[id] = obj;
    if (!obj->mWritePtr && (desc.writeSize || desc.wType == ObjectInfo::Common))
    {
        if (desc.wType == ObjectInfo::String)
        {
            mObjBuffers[id].resize(sizeof(_String));
            obj->mWritePtr = mObjBuffers[id].data();
            obj->mDesc.writeSize = sizeof(_String);
            _String x3;
            for (size_t i=0; i<sizeof(_String); i++)
                reinterpret_cast<unsigned char*>(obj->mWritePtr)[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
        else if (desc.wType == ObjectInfo::Common && desc.writeSize == 0)
        {
            mObjBuffers[id].resize(sizeof(ByteArray));
            obj->mWritePtr = mObjBuffers[id].data();
            ByteArray x3;
            for (size_t i=0; i<sizeof(ByteArray); i++)
                reinterpret_cast<unsigned char*>(obj->mWritePtr)[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
        else
        {
            mObjBuffers[id].resize(desc.writeSize);
            obj->mWritePtr = mObjBuffers[id].data();
        }
    }
    
    if (!obj->mReadPtr && (desc.readSize || desc.rType == ObjectInfo::Common))
    {
        int sz = (desc.rType == ObjectInfo::String)? sizeof(_String): desc.readSize;
        if ((desc.rType == ObjectInfo::Common && desc.readSize == 0))
            sz = sizeof(ByteArray);

        if (desc.flags & ObjectInfo::Dual)
        {
            int osz = mObjBuffers[id].size();
            mObjBuffers[id].resize(osz + sz);
            obj->mWritePtr = mObjBuffers[id].data();
            obj->mReadPtr = mObjBuffers[id].data() + osz;
        }
        else if (desc.writeSize || desc.wType == ObjectInfo::Common)
        {
            obj->mReadPtr = mObjBuffers[id].data();
        }
        else
        {
            mObjBuffers[id].resize(sz);
            obj->mReadPtr = mObjBuffers[id].data();
        }
      
        if (desc.rType == ObjectInfo::String)
        {
            _String x3;
            for (size_t i=0; i<sizeof(_String); i++)
                const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(obj->mReadPtr))[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
        else if (desc.rType == ObjectInfo::Common && desc.readSize == 0)
        {
            ByteArray x3;
            for (size_t i=0; i<sizeof(ByteArray); i++)
                const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(obj->mReadPtr))[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
    }

    bool readyFlag = true;
    for (unsigned int i=0; i<mObjectCount; i++)
    {
        if (i>=mObjects.size() || !mObjects[i])
            readyFlag = false;
    }
    
    if (readyFlag)
    {
        readyEvent();
#ifdef QT_CORE_LIB
        emit ready();
#else 
        if (onReady)
            onReady(this);
#endif
    }
}
//---------------------------------------------------------

void ObjnetDevice::receiveServiceObject(unsigned char oid, const ByteArray &ba)
{  
    // this is should be after data assignment, however noxep.
    if (oid < 32)
        mInfoValidFlags |= (1 << oid);
  
    if (!ba.size())
        return;
  
    switch (oid)
    {
      case svcClass:
        mClass = *reinterpret_cast<const uint32_t*>(ba.data());
        break;
        
      case svcName:
        mName = string(ba.data(), ba.size());
        mName.resize(strlen(mName.c_str()));
       break;

      case svcFullName:
        mFullName = string(ba.data(), ba.size());
        mFullName.resize(strlen(mFullName.c_str()));
        break;

      case svcSerial:
        mSerial = *reinterpret_cast<const uint32_t*>(ba.data());
        break;

      case svcVersion:
        mVersion = *reinterpret_cast<const unsigned short*>(ba.data());
        break;

      case svcBuildDate:
        mBuildDate = string(ba.data(), ba.size());
        break;

      case svcCpuInfo:
        mCpuInfo = string(ba.data(), ba.size());
        break;

      case svcBurnCount:
        mBurnCount = *reinterpret_cast<const uint32_t*>(ba.data());
        break;

      case svcObjectCount:
        mObjectCount = ba[0];
        break;
        
      case svcBusType:
        mBusType = static_cast<BusType>(ba[0]);
        break;

      case svcObjectInfo:
        prepareObject(ba);
        break;

      case svcTimedObject:
        receiveTimedObject(ba);
        break;
        
      case svcGroupedObject:
        receiveGroupedObject(ba);
        break;

      case svcAutoRequest:
      case svcTimedRequest:
      {
        int period = *reinterpret_cast<const int*>(ba.data());
        unsigned char oid = ba[4];
        if (oid < mObjects.size())
        {
            #ifdef QT_CORE_LIB
            emit autoRequestAccepted(mObjects[oid]->name(), period);
            #endif
        }
      } break;

      default:; // warning elimination
    }
    
    #ifdef QT_CORE_LIB
    if (oid < svcObjectInfo)
        emit infoReceived(oid);
    #endif
}

void ObjnetDevice::receiveObject(unsigned char oid, const ByteArray &ba)
{
    mTempTimeout = 0;
    if (oid < mObjects.size())
    {
        ObjectInfo *obj = mObjects[oid];
        if (obj)
        {
            bool res = obj->write(ba);
//            if (!res)
//                qDebug() << "failed to write obj" << obj->name();
            #ifndef QT_CORE_LIB
            if (onObjectReceived)
                onObjectReceived(obj->name());
            #else
            emit objectReceived(obj->name(), obj->toVariant());
            #endif
        }
        else
        {
            #ifdef QT_CORE_LIB
            qDebug() << "no such object";
            #endif
        }
    }
}

void ObjnetDevice::receiveTimedObject(const ByteArray &ba)
{
    unsigned char oid = ba[0];
//    unsigned char reserve = ba[1];
    uint32_t timestamp = *reinterpret_cast<const uint32_t*>(ba.data() + 2);
    ByteArray objba = ba.mid(6);
    if (oid < mObjects.size())
    {
        ObjectInfo *obj = mObjects[oid];
        if (obj)
        {
            bool res = obj->write(objba);
//            if (!res)
//                qDebug() << "failed to write obj" << obj->name();
            #ifndef QT_CORE_LIB
#warning timed object not available in IAR (poka 4to)
            if (onObjectReceived)
                onObjectReceived(obj->name());
            #else
            emit timedObjectReceived(obj->name(), timestamp, obj->toVariant());
            #endif
        }
        else
        {
            #ifdef QT_CORE_LIB
            qDebug() << "no such object";
            #endif
        }
    }
}

void ObjnetDevice::receiveGroupedObject(const ByteArray &ba)
{
    mTempTimeout = 0;
    #ifdef QT_CORE_LIB
    QVariantMap values;
    #endif

    for (int idx=0; idx<ba.size();)
    {
        unsigned char oid = ba[idx++];
//        indices.append(oid);
        if (oid < mObjects.size())
        {
            ObjectInfo *obj = mObjects[oid];
            if (obj)
            {
                int sz = obj->description().writeSize;
                ByteArray objBa = ba.mid(idx, sz);
                idx += sz;
                bool res = obj->write(objBa);
                #ifndef QT_CORE_LIB
                if (onObjectReceived)
                    onObjectReceived(obj->name());
                #else
                values[obj->name()] = obj->toVariant();
                emit objectReceived(obj->name(), obj->toVariant());
                #endif
            }
        }
    }

    #ifndef QT_CORE_LIB
//    if (onObjectGroupReceived)
//        onObjectGroupReceived(indices);
    #else
    emit objectGroupReceived(values);
    #endif
}

void ObjnetDevice::receiveGlobalMessage(unsigned char aid)
{
#ifdef QT_CORE_LIB
    emit globalMessage(aid);
#else
    if (onGlobalMessage)
        onGlobalMessage(aid);
#endif
}

void ObjnetDevice::onObjectValueChanged(unsigned char id)
{
    #ifdef QT_CORE_LIB
    ObjectInfo *obj = mObjects[id];
    emit objectValueChanged(obj->name(), obj->toVariant());
    #endif
}
//---------------------------------------------------------

void ObjnetDevice::requestObject(_String name)
{    
    map<string, ObjectInfo>::iterator it = mObjMap.find(_fromString(name));
    if (it != mObjMap.end() && it->second.flags())
    {
        unsigned char oid = it->second.mDesc.id;
        #ifdef QT_CORE_LIB
        emit requestObject(mNetAddress, oid);
        #else
        masterRequestObject(mNetAddress, oid);
        #endif
    }
    
    mTempTimeout = 1;
}

void ObjnetDevice::sendObject(_String name)
{
    map<string, ObjectInfo>::iterator it = mObjMap.find(_fromString(name));
    if (it != mObjMap.end() && it->second.flags())
    {
        unsigned char oid = it->second.mDesc.id;
        ObjectInfo *obj = mObjects[oid];
        if (obj)
        {
            #ifdef QT_CORE_LIB
            emit sendObject(mNetAddress, oid, obj->read());
            #else
            masterSendObject(mNetAddress, oid, obj->read());
            #endif
        }
    }
}

void ObjnetDevice::groupedRequest(std::vector<_String> names)
{   
    ByteArray ba;
    int cnt = names.size();
    for (int i=0; i<cnt; i++)
    {
        _String name = names[i];
        map<string, ObjectInfo>::iterator it = mObjMap.find(_fromString(name));
        if (it != mObjMap.end() && it->second.flags())
            ba.append(it->second.mDesc.id);
    }
    #ifdef QT_CORE_LIB
    emit serviceRequest(mNetAddress, svcGroupedRequest, ba);
    #else
    masterServiceRequest(mNetAddress, svcGroupedRequest, ba);
    #endif
    
    mTempTimeout = 1;
}

void ObjnetDevice::autoRequest(_String name, int periodMs)
{
    map<string, ObjectInfo>::iterator it = mObjMap.find(_fromString(name));
    if (it != mObjMap.end() && it->second.flags())
    {
        unsigned char oid = it->second.mDesc.id;
        ByteArray ba;
        ba.append(reinterpret_cast<const char*>(&periodMs), sizeof(int));
        ba.append(oid);
        #ifdef QT_CORE_LIB
        emit serviceRequest(mNetAddress, svcAutoRequest, ba);
        #endif
    }
}

void ObjnetDevice::timedRequest(_String name, int periodMs)
{
    map<string, ObjectInfo>::iterator it = mObjMap.find(_fromString(name));
    if (it != mObjMap.end() && it->second.flags())
    {
        unsigned char oid = it->second.mDesc.id;
        ByteArray ba;
        ba.append(reinterpret_cast<const char*>(&periodMs), sizeof(int));
        ba.append(oid);
        #ifdef QT_CORE_LIB
        emit serviceRequest(mNetAddress, svcTimedRequest, ba);
        #endif
    }
}

void ObjnetDevice::requestInfo(unsigned char oid)
{
    if (mMaster)
    {
        mMaster->sendServiceRequest(mNetAddress, (SvcOID)oid, ByteArray());
    }
}

void ObjnetDevice::requestMetaInfo()
{
    if (mMaster)
    {
        mMaster->requestDevInfo(mNetAddress);
//        mMaster->requestObjInfo(mNetAddress);
    }
}

#ifdef QT_CORE_LIB
void ObjnetDevice::sendObject(QString name, QVariant value)
{
    map<string, ObjectInfo>::iterator it = mObjMap.find(_fromString(name));
    if (it != mObjMap.end() && it->second.flags())
    {
        unsigned char oid = it->second.mDesc.id;
        ObjectInfo *obj = mObjects[oid];
        if (obj)
        {
            if (obj->rType() == ObjectInfo::Common)
                value = QByteArray::fromHex(value.toByteArray());
            else
                value.convert(obj->rType());
            obj->fromVariant(value);
            emit sendObject(mNetAddress, oid, obj->read());
        }
    }
}
#endif
//---------------------------------------------------------

_String ObjnetDevice::busTypeName() const
{
    switch (mBusType)
    {
        case BusUnknown:    return "unknown";
        case BusCan:        return "CAN";
        case BusUsbHid:     return "USB HID";
        case BusWifi:       return "WiFi";
        case BusSwonb:      return "SWONB";
        case BusVirtual:    return "virtual";
        case BusRadio:      return "radio";
        default:            return "unknown";
    }
}
//---------------------------------------------------------

void ObjnetDevice::changeName(_String name)
{
    if (mMaster)
    {
#ifdef QT_CORE_LIB
        ByteArray ba(name.toLocal8Bit());
#else
        ByteArray ba(name.c_str());
#endif
        if (ba.size() > 8)
            ba.resize(8);
        mMaster->sendServiceRequest(mNetAddress, svcName, ba);
        mName = _fromString(name);
    }
}

void ObjnetDevice::changeFullName(_String name)
{
    if (mMaster)
    {
#ifdef QT_CORE_LIB
        ByteArray ba(name.toUtf8());
#else
        ByteArray ba(name.c_str());
#endif
        if (ba.size() > 32)
            ba.resize(32);
        mMaster->sendServiceRequest(mNetAddress, svcFullName, ba);
        mFullName = _fromString(name);
    }
}

void ObjnetDevice::changeBusAddress(unsigned char mac)
{
    if (mMaster)
    {
        ByteArray ba;
        ba.append(mac);
        mMaster->sendServiceRequest(mNetAddress, svcBusAddress, ba);
        mBusAddress = mac;
    }
}
//---------------------------------------------------------
