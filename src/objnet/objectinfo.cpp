#include "objectinfo.h"

using namespace Objnet;

int ObjectInfo::mAssignId = 0;

ObjectInfo::ObjectInfo() :
    mReadPtr(0L), mWritePtr(0L),
    mAutoPeriod(0), mAutoTime(0),
    mTimedRequest(false),
    mIsDevice(false)
{
}

#ifndef QT_CORE_LIB
template<> ObjectInfo::ObjectInfo<void>(string name, Closure<void(void)> event, ObjectInfo::Flags flags) :
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mTimedRequest(false),
    mIsDevice(false)
{
    mDesc.readSize = 0; // no return
    mDesc.writeSize = 0; // no param
        
    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<void(void)>*>(&mReadPtr) = event;
    
    mDesc.rType = Void; // return type
    mDesc.wType = Void; // param type
    mDesc.flags = (flags | Function) & ~(Save | Write);
    mDesc.name = name;
    mDesc.id = mAssignId++;
}
#endif
//---------------------------------------------------------

ByteArray ObjectInfo::read()
{
    if ((mDesc.flags & Function) && !mIsDevice)
        return invoke(ByteArray());
    
    if (!mDesc.readSize || !mReadPtr || !(mDesc.flags & Read))
        return ByteArray();
    if (mDesc.rType == String)
    {
        const _String *str = reinterpret_cast<const _String*>(mReadPtr);
        if (str)
        {
            string s = _fromString(*str);
            return ByteArray(s.c_str(), s.length());
        }
        return ByteArray();
    }
    else
    {
        return ByteArray(reinterpret_cast<const char*>(mReadPtr), mDesc.readSize);
    }
}

bool ObjectInfo::write(const ByteArray &ba)
{
    if ((mDesc.flags & Function) && !mIsDevice)
        invoke(ba);
  
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

ByteArray ObjectInfo::invoke(const ByteArray &ba)
{
    if (!(mDesc.flags & Function) || mIsDevice)
        return ByteArray();
   
    ByteArray ret;
    switch (mDesc.rType)
    {
        case Void: // just call the method
            switch (mDesc.wType)
            {
                case Void: (*reinterpret_cast<Closure<void(void)>*>(&mReadPtr))(); break;
                #define CASE(Tp) case Tp: (*reinterpret_cast<Closure<void(Tp##_t)>*>(&mReadPtr))(*reinterpret_cast<const Tp##_t*>(ba.data())); break
                CASE(Bool);
                CASE(Int);
                CASE(UInt);
                CASE(LongLong);
                CASE(ULongLong);
                CASE(Double);
                CASE(Long);
                CASE(Short);
                CASE(Char);
                CASE(ULong);
                CASE(UShort);
                CASE(UChar);
                CASE(Float);
                CASE(SChar);
                case String: (*reinterpret_cast<Closure<void(string)>*>(&mReadPtr))(string(ba.data(), ba.size())); break;
                #undef CASE
            }
            break;
    
        #define CASEw(Tr, Tp)  case Tp: result = (*reinterpret_cast<Closure<Tr##_t(Tp##_t)>*>(&mReadPtr))(*reinterpret_cast<const Tp##_t*>(ba.data())); break        
        #define CASE(Tr) case Tr: { \
            Tr##_t result; \
            switch (mDesc.wType) \
            { \
                case Void: result = (*reinterpret_cast<Closure<Tr##_t(void)>*>(&mReadPtr))(); break; \
                CASEw(Tr, Bool); \
                CASEw(Tr, Int); \
                CASEw(Tr, UInt); \
                CASEw(Tr, LongLong); \
                CASEw(Tr, ULongLong); \
                CASEw(Tr, Double); \
                CASEw(Tr, Long); \
                CASEw(Tr, Short); \
                CASEw(Tr, Char); \
                CASEw(Tr, ULong); \
                CASEw(Tr, UShort); \
                CASEw(Tr, UChar); \
                CASEw(Tr, Float); \
                CASEw(Tr, SChar); \
                case String: result = (*reinterpret_cast<Closure<Tr##_t(string)>*>(&mReadPtr))(string(ba.data(), ba.size())); break; \
            } \
            ret.append(reinterpret_cast<const char*>(&result), sizeof(Tr##_t)); \
        } break
    
        CASE(Bool);
        CASE(Int);
        CASE(UInt);
        CASE(LongLong);
        CASE(ULongLong);
        CASE(Double);
        CASE(Long);
        CASE(Short);
        CASE(Char);
        CASE(ULong);
        CASE(UShort);
        CASE(UChar);
        CASE(Float);
        CASE(SChar);
        
        case String:
          {
            string result;
            switch (mDesc.wType)
            {
                case Void: result = (*reinterpret_cast<Closure<string(void)>*>(&mReadPtr))(); break;
                CASEw(String, Bool); \
                CASEw(String, Int); \
                CASEw(String, UInt); \
                CASEw(String, LongLong); \
                CASEw(String, ULongLong); \
                CASEw(String, Double); \
                CASEw(String, Long); \
                CASEw(String, Short); \
                CASEw(String, Char); \
                CASEw(String, ULong); \
                CASEw(String, UShort); \
                CASEw(String, UChar); \
                CASEw(String, Float); \
                CASEw(String, SChar); \
                case String: result = (*reinterpret_cast<Closure<string(string)>*>(&mReadPtr))(string(ba.data(), ba.size())); break;
            }
            ret.append(result.c_str(), result.length()); 
          } break;
//                    Common = 12,
        
    }
    #undef CASE
    #undef CASEw
    return ret;
}
//---------------------------------------------------------

int ObjectInfo::sizeofType(ObjectInfo::Type type) const
{
    switch (type)
    {
        case Void:      return 0;
        case Bool:      return 1;
        case Int:       return 4;
        case UInt:      return 4;
        case LongLong:  return 8;
        case ULongLong: return 8;
        case Double:    return 8;
        case Long:      return 4;
        case Short:     return 2;
        case Char:      return 1;
        case ULong:     return 4;
        case UShort:    return 2;
        case UChar:     return 1;
        case Float:     return 4;
        case SChar:     return 1;

        default:        return 0;
//        String : return 0,  // QString B Qt, string B APMe
//        Common : return 0, // Common - this is (Q)ByteArray
    }
}
//---------------------------------------------------------

#ifdef QT_CORE_LIB
QVariant ObjectInfo::toVariant()
{
    if (!mWritePtr)
        return QVariant(); // "Invalid" type
    if (mDesc.wType == Common)
        return ByteArray(reinterpret_cast<const char*>(mWritePtr), mDesc.writeSize);
//    if (mDesc.type == String)
//        return *reinterpret_cast<_String*>(mWritePtr);


    if (mDesc.flags & Array)
    {
        int sz = sizeofType((Type)mDesc.wType);
        if (sz)
        {
            int N = mDesc.writeSize / sz;
            QList<QVariant> vec;
            for (int i=0; i<N; i++)
                vec << QVariant(mDesc.wType, reinterpret_cast<const char*>(mWritePtr) + sz*i);
            return vec;
        }
        return QVariant();
    }
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
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(mReadPtr))[i] = ba[i];
        return true;
    }
    if (mDesc.flags & Array)
    {
        QVariantList list = v.toList();
        int sz = sizeofType((Type)mDesc.rType);
        int N = mDesc.readSize / sz;
        for (int j=0; j<N && j<list.size(); j++)
        {
            QVariant v = list[j];
            for (int i=0; i<mDesc.readSize; i++)
                const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(mReadPtr))[i+j*sz] = reinterpret_cast<unsigned char*>(v.data())[i];
        }
    }
    else
    {
        for (int i=0; i<mDesc.readSize; i++)
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(mReadPtr))[i] = reinterpret_cast<unsigned char*>(v.data())[i];
    }
    return true;
}
#endif
//---------------------------------------------------------
