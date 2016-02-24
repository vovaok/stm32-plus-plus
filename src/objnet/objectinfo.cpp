#include "objectinfo.h"

using namespace Objnet;

ObjectInfo::ObjectInfo() :
    mReadPtr(0L), mWritePtr(0L)
{
}

//ObjectInfo::ObjectInfo(string name, NotifyEvent var, ObjectInfo::Flags flags) :
//    mReadPtr(0), mWritePtr(0)
//{
//    size_t sz = 0;
//
//    if (flags & Read)
//    {
//        mReadPtr = 0;//var;
//        mDesc.readSize = sz;
//    }
//    if (flags & Write)
//    {
//        mWritePtr = 0;
//        mDesc.writeSize = 0;
//    }
//
//    mDesc.type = 0;//typeOfVar(var);
//    mDesc.flags = flags;
//    mDesc.name = name;
//}
//---------------------------------------------------------

ByteArray ObjectInfo::read()
{
    if (!mDesc.readSize || !mReadPtr || !(mDesc.flags & Read))
        return ByteArray();
    if (mDesc.rType == String)
    {
        string *str = reinterpret_cast<string*>(mReadPtr);
        if (str)
            return ByteArray(str->c_str(), str->length());
        return ByteArray();
    }
    else
    {
        return ByteArray(reinterpret_cast<const char*>(mReadPtr), mDesc.readSize);
    }
}

bool ObjectInfo::write(const ByteArray &ba)
{
    if (!mDesc.writeSize || !mWritePtr || !(mDesc.flags & Write))
        return false;
    if (mDesc.wType == String)
    {
        _String *str = reinterpret_cast<_String*>(mWritePtr);
        if (!str)
            return false;
        else
        {
            *str = _toString(string(ba.data(), ba.size()));
        }
        return true;
    }
    else if ((size_t)ba.size() == mDesc.writeSize)
    {
        for (size_t i=0; i<mDesc.writeSize; i++)
            reinterpret_cast<unsigned char*>(mWritePtr)[i] = ba[i];
        return true;
    }
    return false;
}
//---------------------------------------------------------

#ifndef __ICCARM__
QVariant ObjectInfo::toVariant()
{
    if (!mWritePtr)
        return QVariant(); // "Invalid" type
    if (mDesc.wType == Common)
        return ByteArray(reinterpret_cast<const char*>(mWritePtr), mDesc.writeSize);
//    if (mDesc.type == String)
//        return *reinterpret_cast<_String*>(mWritePtr);

    return QVariant(mDesc.wType, mWritePtr);
}

bool ObjectInfo::fromVariant(QVariant &v)
{
    if (mDesc.rType != v.type())
        return false;
    if (mDesc.rType == Common)
    {
        ByteArray ba = v.toByteArray();
        int sz = ba.size();
        sz = sz > mDesc.readSize? mDesc.readSize: sz;
        for (int i=0; i<sz; i++)
            reinterpret_cast<unsigned char*>(mReadPtr)[i] = ba[i];
        return true;
    }
    for (int i=0; i<mDesc.readSize; i++)
        reinterpret_cast<unsigned char*>(mReadPtr)[i] = reinterpret_cast<unsigned char*>(v.data())[i];
    return true;
}
#endif
//---------------------------------------------------------
