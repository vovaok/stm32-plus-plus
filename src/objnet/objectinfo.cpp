#include "objectinfo.h"

using namespace Objnet;

ObjectInfo::ObjectInfo() :
    mReadPtr(0L), mWritePtr(0L)
{
}

//template<typename T>
//ObjectInfo::ObjectInfo(string name, T &var, Flags flags) :
//    mReadPtr(0), mWritePtr(0)
//{
//    if (flags & Read)
//    {
//        mReadPtr = &var;
//        mDesc.readSize = sizeof(T);
//    }
//    if (flags & Write)
//    {
//        mWritePtr = &var;
//        mDesc.writeSize = sizeof(T);
//    }
//    
//    mDesc.type = Int;
//    mDesc.flags = flags;
//    mDesc.name = name;
//}
//---------------------------------------------------------

//ObjectInfo &ObjectInfo::bindVariableRO(string name, void *ptr, size_t size)
//{
//    mReadPtr = ptr;
//    mDesc.readSize = size;
//    mWritePtr = 0;
//    mDesc.writeSize = 0;
//    mDesc.type = Common;
//    mDesc.flags = ReadOnly;
//    mDesc.name = name;
//    return *this;
//}

//ObjectInfo &ObjectInfo::bindVariableWO(string name, void *ptr, size_t size)
//{
//    mReadPtr = 0;
//    mDesc.readSize = 0;
//    mWritePtr = ptr;
//    mDesc.writeSize = size;
//    mDesc.type = Common;
//    mDesc.flags = WriteOnly;
//    mDesc.name = name;
//    return *this;
//}

//ObjectInfo &ObjectInfo::bindVariableRW(string name, void *ptr, size_t size)
//{
//    mReadPtr = mWritePtr = ptr;
//    mDesc.readSize = mDesc.writeSize = size;
//    mDesc.type = Common;
//    mDesc.flags = ReadWrite;
//    mDesc.name = name;
//    return *this;
//}

//ObjectInfo &ObjectInfo::bindVariableInOut(string name, void *inPtr, size_t inSize, void *outPtr, size_t outSize)
//{
//    mReadPtr = outPtr;
//    mDesc.readSize = outSize;
//    mWritePtr = inPtr;
//    mDesc.writeSize = inSize;
//    mDesc.type = Common;
//    mDesc.flags = ReadWrite;
//    mDesc.name = name;
//    return *this;
//}

//ObjectInfo &ObjectInfo::bindString(string name, string *str)
//{
//    mReadPtr = mWritePtr = str;
//    mDesc.readSize = mDesc.writeSize = 1;
//    mDesc.type = String;
//    mDesc.flags = ReadWrite;
//    mDesc.name = name;
//    return *this;
//}
//---------------------------------------------------------

ByteArray ObjectInfo::read()
{
    if (!mDesc.readSize || !mReadPtr || !(mDesc.flags & Read))
        return ByteArray();
    if (mDesc.type == String)
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
    if (mDesc.type == String)
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
    if (mDesc.type == Common)
        return ByteArray(reinterpret_cast<const char*>(mWritePtr), mDesc.writeSize);
//    if (mDesc.type == String)
//        return *reinterpret_cast<_String*>(mWritePtr);

    return QVariant(mDesc.type, mWritePtr);
}

bool ObjectInfo::fromVariant(QVariant &v)
{
    if (mDesc.type != v.type())
        return false;
    if (mDesc.type == Common)
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
