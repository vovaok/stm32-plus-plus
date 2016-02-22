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
    unsigned char t = desc.type;
    if (obj->mWritePtr && desc.type != obj->mDesc.type)
    {
        t = obj->mDesc.type;
//        obj->mWritePtr = 0L;
        #ifndef __ICCARM__
        qDebug() << "type mismatch!!";
        #endif
    }

    obj->mDesc = desc;
    obj->mDesc.type = t;
    unsigned char id = desc.id;
    if (id >= mObjects.size())
    {
        mObjects.resize(id + 1);
        mObjBuffers.resize(id + 1);
    }

    mObjects[id] = obj;
    if (!obj->mWritePtr)
    {
        if (desc.type == ObjectInfo::String)
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

    obj->mReadPtr = obj->mWritePtr;
//  qDebug() << id << ":" << QString::fromStdString(desc.name);
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
