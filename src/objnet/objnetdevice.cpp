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
    #ifndef QT_CORE_LIB
    mSendTimer.setTimeoutEvent(EVENT(&ObjnetDevice::onSendTimer));
    #else
    QObject::connect(&mSendTimer, SIGNAL(timeout()), this, SLOT(onSendTimer()));
    #endif
    mSendTimer.start(1);
}
//---------------------------------------------------------

void ObjnetDevice::parseObjectInfo(const ByteArray &ba)
{
    ObjectInfo *obj = 0L;
    ByteArray idba;
    if ((uint8_t)ba[0] != 0xFF)
    {
        obj = prepareObject(ba);
    }
    else
    {
        obj = mObjects[ba[1]];
        idba.append(obj->id());
        ByteArray baba = ba.mid(2);
        while (obj && baba[0] == (char)0xFF)
        {
            obj = &obj->subobject(baba[1]);
            idba.append(obj->id());
            baba.remove(0, 2);
        }
        
        if (obj)
        {
            uint8_t subid = baba[0];
            ObjectInfo &sub = obj->subobject(subid);
            sub.mDesc = baba;
            uint8_t roff=0, woff=0;
            for (int i=0; i<obj->subobjectCount(); i++)
            {
                if (i > 0)
                {
                    ObjectInfo &prev = obj->subobject(i-1);
                    if (!prev.isValid())
                        break;
                    if (obj->mDesc.flags | ObjectInfo::ReadOnly)
                        roff += obj->subobject(i).mDesc.readSize;
                    if (obj->mDesc.flags | ObjectInfo::WriteOnly)
                        woff += obj->subobject(i).mDesc.writeSize;
                }
                // update all pointers
                ObjectInfo &cur = obj->subobject(i);
                if (obj->mDesc.flags | ObjectInfo::ReadOnly)
                    cur.mReadPtr = (uint8_t*)obj->mReadPtr + roff;
                if (obj->mDesc.flags | ObjectInfo::WriteOnly)
                    cur.mWritePtr = (uint8_t*)obj->mWritePtr + woff;
            }
//            if (obj->mDesc.flags | ObjectInfo::ReadOnly)
//                sub.mReadPtr = (uint8_t*)obj->mReadPtr + roff;
//            if (obj->mDesc.flags | ObjectInfo::WriteOnly)
//                sub.mWritePtr = (uint8_t*)obj->mWritePtr + woff;
            
            if (sub.isCompound())
            {
                uint8_t cnt = sub.mDesc.rType - (uint8_t)ObjectInfo::Compound;
                sub.m_subobjects.resize(cnt);
            }

            sub.mIsDevice = true;
            sub.mValid = true;
            sub.m_parentObject = obj;
            obj = &sub;
        }
    }
    
    if (isReady())
    {
        readyEvent();
#ifdef QT_CORE_LIB
        emit ready();
#else 
        if (onReady)
            onReady(this);
#endif
    }
    
    // don't request subobjects! node should send it by own means
    
//    if (obj && obj->isCompound())
//    {
//        idba.append(obj->id());
//        int subsz = obj->mDesc.rType - (uint8_t)ObjectInfo::Compound;
//        int idx = idba.size();
//        idba.append((char)0);
//        for (int i=0; i<subsz; i++)
//        {
//            idba[idx] = i;
//            #ifdef QT_CORE_LIB
//            emit serviceRequest(mNetAddress, svcObjectInfo, idba);
//            #else
//            masterServiceRequest(mNetAddress, svcObjectInfo, idba);
//            #endif
//        }
//    }
}

ObjectInfo *ObjnetDevice::prepareObject(const ObjectInfo::Description &desc)
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
//    if (id >= mObjects.size())
//    {
//        int sz = mObjects.size();
//        mObjects.resize(id + 1);
//        mObjBuffers.resize(id + 1);
//        for (int i=0; i<sz; i++)
//        {
//            ObjectInfo *o = mObjects[i];
//            if (o)
//            {
//                char *ptr = mObjBuffers[i].data();
//                if (o->mWritePtr && o->mWritePtr != ptr)
//                {
//                    if (o->mReadPtr == o->mWritePtr)
//                        o->mReadPtr = o->mWritePtr = ptr;
//                    else if (!o->mReadPtr)
//                        o->mWritePtr = ptr;
//                    else
//                    {
//                        uintptr_t off = (uintptr_t)o->mReadPtr - (uintptr_t)o->mWritePtr;
//                        o->mWritePtr = ptr;
//                        o->mReadPtr = ptr + off;
//                    }
//                }
//                else if (o->mReadPtr && o->mReadPtr != ptr)
//                {
//                    o->mReadPtr = ptr;
//                }
//            }
//        }
//        for (int i=sz; i<id; i++)
//            mObjects[i] = 0L;
//    }
    
    bool isArray = desc.flags & ObjectInfo::Array;
    
    bool wIsCommonType = (desc.wType == ObjectInfo::Common || obj->isCompound());
    bool rIsCommonType = (desc.rType == ObjectInfo::Common || obj->isCompound());
    
    bool wIsBuffer = (!desc.writeSize && isArray);
    bool rIsBuffer = (!desc.readSize && isArray);
    
    if (desc.rType >= ObjectInfo::Compound)
    {
        uint8_t cnt = desc.rType - (uint8_t)ObjectInfo::Compound;
        obj->m_subobjects.resize(cnt);
    }

    mObjects[id] = obj;
    
    if (!obj->mWritePtr && (desc.writeSize || wIsCommonType || wIsBuffer))
    {
        if (desc.wType == ObjectInfo::String)
        {
            int N = 1;
            if (isArray)
                N = obj->mDesc.writeSize;
            mObjBuffers[id].resize(sizeof(_String) * N);
            obj->mWritePtr = mObjBuffers[id].data();
            obj->mDesc.writeSize = N;//sizeof(_String);
            if (N > 1)
                new (obj->mWritePtr) _String[N];
            else
                new (obj->mWritePtr) _String;
//                _String x3;
//                for (size_t i=0; i<sizeof(_String); i++)
//                    reinterpret_cast<unsigned char*>(obj->mWritePtr)[i] = reinterpret_cast<unsigned char*>(&x3)[i];
            
        }
        else if (wIsCommonType && desc.writeSize == 0)
        {
            mObjBuffers[id].resize(sizeof(ByteArray));
            obj->mWritePtr = mObjBuffers[id].data();
            ByteArray x3;
            for (size_t i=0; i<sizeof(ByteArray); i++)
                reinterpret_cast<unsigned char*>(obj->mWritePtr)[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
        else if (wIsBuffer)
        {
            if (desc.wType == ObjectInfo::Float)
            {
                RingBuffer<float> ring(20);
                mObjBuffers[id].resize(sizeof(RingBuffer<float>));
                obj->mWritePtr = mObjBuffers[id].data();
                *reinterpret_cast<RingBuffer<float>*>(obj->mWritePtr) = ring;
            }
        }
        else
        {
            mObjBuffers[id].resize(desc.writeSize);
            obj->mWritePtr = mObjBuffers[id].data();
        }
    }
    
    if (!obj->mReadPtr && (desc.readSize || rIsCommonType || rIsBuffer))
    {
        int sz = desc.readSize;
        if (desc.rType == ObjectInfo::String)
        {
            if (isArray)
                sz = sizeof(_String) * desc.readSize;
            else 
                sz = sizeof(_String);
        }
        else if ((rIsCommonType && desc.readSize == 0))
            sz = sizeof(ByteArray);
        else if (rIsBuffer)
            sz = sizeof(RingBuffer<float>);

        if (desc.flags & ObjectInfo::Dual)
        {
            int osz = mObjBuffers[id].size();
            mObjBuffers[id].resize(osz + sz);
            obj->mWritePtr = mObjBuffers[id].data();
            obj->mReadPtr = mObjBuffers[id].data() + osz;
        }
        else if (desc.writeSize || wIsCommonType)
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
            if (isArray)
                new ((void*)obj->mReadPtr) _String[desc.readSize];
            else
                new ((void*)obj->mReadPtr) _String;
//            _String x3;
//            for (size_t i=0; i<sizeof(_String); i++)
//                const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(obj->mReadPtr))[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
        else if (rIsCommonType && desc.readSize == 0)
        {
            ByteArray x3;
            for (size_t i=0; i<sizeof(ByteArray); i++)
                const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(obj->mReadPtr))[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
        else if (rIsBuffer)
        {
            if (desc.rType == ObjectInfo::Float)
            {
                RingBuffer<float> ring(20);
                *reinterpret_cast<RingBuffer<float>*>(const_cast<void *>(obj->mReadPtr)) = ring;
            }
        }
    }

    obj->mValid = true;
    
    return obj;
}

bool ObjnetDevice::isObjectInfoValid() const
{
    for (unsigned int i=0; i<mObjectCount; i++)
    {
        if (i>=mObjects.size() || !mObjects[i])
            return false;
        if (!mObjects[i]->isValid())
            return false;
    }
    return true;
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
        mObjects.resize(mObjectCount);
        for (int i=0; i<mObjectCount; i++)
            mObjects[i] = 0L;
        mObjBuffers.resize(mObjectCount);
        break;
        
      case svcBusType:
        mBusType = static_cast<BusType>(ba[0]);
        break;

      case svcObjectInfo:
        parseObjectInfo(ba);
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
            /*bool res =*/ obj->write(ba);
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
//#warning timed object not available in IAR (poka 4to)
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
        ba.append(reinterpret_cast<const char*>(&periodMs), sizeof(uint32_t));
        ba.append(oid);

        if (mBusType == BusSwonb)
        {
            if (periodMs >= 0)
            {
                mObjects[oid]->mAutoPeriod = periodMs;
                mObjects[oid]->mTimedRequest = false;
            }
            else
            {
                *reinterpret_cast<uint32_t*>(ba.data()) = mObjects[oid]->mAutoPeriod;
            }
            receiveServiceObject(svcAutoRequest, ba);
        }
        else
        {
            #ifdef QT_CORE_LIB
            emit serviceRequest(mNetAddress, svcAutoRequest, ba);
            #else
            masterServiceRequest(mNetAddress, svcAutoRequest, ba);
            #endif
        }
    }
}

void ObjnetDevice::timedRequest(_String name, int periodMs)
{
    map<string, ObjectInfo>::iterator it = mObjMap.find(_fromString(name));
    if (it != mObjMap.end() && it->second.flags())
    {
        unsigned char oid = it->second.mDesc.id;
        ByteArray ba;
        ba.append(reinterpret_cast<const char*>(&periodMs), sizeof(uint32_t));
        ba.append(oid);

        if (mBusType == BusSwonb)
        {
            if (periodMs >= 0)
            {
                mObjects[oid]->mAutoPeriod = periodMs;
                mObjects[oid]->mTimedRequest = true;
            }
            else
            {
                *reinterpret_cast<uint32_t*>(ba.data()) = mObjects[oid]->mAutoPeriod;
            }
            receiveServiceObject(svcTimedRequest, ba);
        }
        else
        {
            #ifdef QT_CORE_LIB
            emit serviceRequest(mNetAddress, svcTimedRequest, ba);
            #else
            masterServiceRequest(mNetAddress, svcTimedRequest, ba);
            #endif
        }
    }
}

void ObjnetDevice::requestInfo(unsigned char oid)
{
    if (mMaster)
    {
        mMaster->sendServiceRequest(mNetAddress, (SvcOID)oid, ByteArray());
    }
}

void ObjnetDevice::requestObjectInfo(unsigned char oid)
{
    if (mMaster)
    {
        ByteArray ba;
        ba.append(oid);
        mMaster->sendServiceRequest(mNetAddress, svcObjectInfo, ba);
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
            if (obj->rType() == ObjectInfo::Common && obj->readSize())
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

void ObjnetDevice::onSendTimer()
{
    for (unsigned char oid=0; oid<mObjects.size(); oid++)
    {
        ObjectInfo *obj = mObjects[oid];
        if (obj && obj->mAutoPeriod)
        {
            obj->mAutoTime++;
            if (obj->mAutoTime >= obj->mAutoPeriod)
            {
                obj->mAutoTime = 0;
                if (obj->mTimedRequest)
                {
                    ByteArray ba;
                    ba.append(oid);
                    #ifdef QT_CORE_LIB
                    emit serviceRequest(mNetAddress, svcGetTimedObject, ba);
                    #else
                    masterServiceRequest(mNetAddress, svcGetTimedObject, ba);
                    #endif
                }
                else 
                {
                    #ifdef QT_CORE_LIB
                    emit requestObject(mNetAddress, oid);
                    #else
                    masterRequestObject(mNetAddress, oid);
                    #endif
                }
            }
        }
    }
//    mTimestamp++;
}
