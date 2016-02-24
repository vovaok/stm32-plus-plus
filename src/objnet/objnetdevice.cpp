#include "objnetdevice.h"

using namespace Objnet;

ObjnetDevice::ObjnetDevice(unsigned char netaddr) :
    mClassValid(false),
    mNameValid(false),
    mNetAddress(netaddr),
    mPresent(false),
    mTimeout(5),
    mAutoDelete(false)
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
    unsigned char wt = desc.wType;
    unsigned char rt = desc.rType;
    
    if (obj->mWritePtr && desc.writeSize && desc.wType != obj->mDesc.wType)
    {
        wt = obj->mDesc.wType;
        #ifndef __ICCARM__
        qDebug() << "[ObjnetDevice::prepareObject] write type mismatch!!";
        #endif
    }
    
    if (obj->mReadPtr && desc.readSize && desc.rType != obj->mDesc.rType)
    {
        rt = obj->mDesc.rType;
        #ifndef __ICCARM__
        qDebug() << "[ObjnetDevice::prepareObject] read type mismatch!!";
        #endif
    }

    obj->mDesc = desc;
    obj->mDesc.wType = wt;
    obj->mDesc.rType = rt;
    unsigned char id = desc.id;
    if (id >= mObjects.size())
    {
        mObjects.resize(id + 1);
        mObjBuffers.resize(id + 1);
    }

    mObjects[id] = obj;
    if (!obj->mWritePtr)
    {
        if (desc.wType == ObjectInfo::String)
        {
            mObjBuffers[id].resize(sizeof(_String));
            obj->mWritePtr = mObjBuffers[id].data();
            _String x3;
            for (size_t i=0; i<sizeof(_String); i++)
                reinterpret_cast<unsigned char*>(obj->mWritePtr)[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
        else
        {
            mObjBuffers[id].resize(desc.writeSize);
            obj->mWritePtr = mObjBuffers[id].data();
        }
    }
    
    if (!obj->mReadPtr)
    {
        int sz = (desc.rType == ObjectInfo::String)? sizeof(_String): desc.readSize;
        if (desc.flags & ObjectInfo::Dual)
        {
            int osz = mObjBuffers[id].size();
            mObjBuffers[id].resize(osz + sz);
            obj->mReadPtr = mObjBuffers[id].data() + osz;
        }
        else
        {
            obj->mReadPtr = mObjBuffers[id].data();
        }
      
        if (desc.rType == ObjectInfo::String)
        {
            _String x3;
            for (size_t i=0; i<sizeof(_String); i++)
                reinterpret_cast<unsigned char*>(obj->mReadPtr)[i] = reinterpret_cast<unsigned char*>(&x3)[i];
        }
    }
}
//---------------------------------------------------------

void ObjnetDevice::receiveObject(unsigned char oid, const ByteArray &ba)
{
    if (oid < mObjects.size())
    {
        ObjectInfo *obj = mObjects[oid];
        if (obj)
        {
            obj->write(ba);
            #ifdef __ICCARM__
            //onObjectReceived(oid);
            #else
            emit objectReceived(obj->name(), obj->toVariant());
            #endif
        }
        else
        {
            #ifndef __ICCARM__
            qDebug() << "no such object";
            #endif
        }
    }
}
//---------------------------------------------------------



void ObjnetDevice::requestObject(_String name)
{
    if (mObjMap.count(_fromString(name)))
    {
        unsigned char oid = mObjMap[_fromString(name)].mDesc.id;
        #ifndef __ICCARM__
        emit requestObject(mNetAddress, oid);
        #endif
    }
}

void ObjnetDevice::sendObject(_String name)
{
    if (mObjMap.count(_fromString(name)))
    {
        unsigned char oid = mObjMap[_fromString(name)].mDesc.id;
        ObjectInfo *obj = mObjects[oid];
        if (obj)
        {
            #ifndef __ICCARM__
            emit sendObject(mNetAddress, oid, obj->read());
            #endif
        }
    }
}
//---------------------------------------------------------
