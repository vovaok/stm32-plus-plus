#ifndef OBJECTINFO_H
#define OBJECTINFO_H

#include "objnetcommon.h"

#ifdef __ICCARM__
#define _String string
#define _toString(x) (x)
#define _fromString(x) (x)
#else
#define _String QString
#define _toString(x) QString::fromStdString(x)
#define _fromString(x) x.toStdString()
#endif

using namespace std;

namespace Objnet
{
  


class ObjectInfo
{
public:  
    typedef enum
    {
        Volatile= 0x01,
        Read    = 0x02,
        Write   = 0x04,
        Save    = 0x08,
        Hidden  = 0x10,
        Dual    = 0x20, // read from one location, write to another
        Function= 0x40, // function call
        
        Constant    = Read,
        ReadOnly    = Read,
        Measurement = Read | Volatile,
        WriteOnly   = Write,
        Control     = Write | Volatile,
        ReadWrite   = Read | Write,
        Exchange    = ReadWrite | Volatile,
        SecretVar   = ReadWrite | Hidden,
        Storage     = ReadWrite | Save,
        SecretSetting = Storage | Hidden
    } Flags;
    
    typedef enum // KAK QVariant / QMetaType B Qt!!!
    {
        Void = 43,
        Bool = 1,
        Int = 2,
        UInt = 3,
        LongLong = 4,
        ULongLong = 5,
        Double = 6,
        Long = 32,
        Short = 33,
        Char = 34,
        ULong = 35,
        UShort = 36,
        UChar = 37,
        Float = 38,
        SChar = 40,
      
        String = 10,  // QString B Qt, string B APMe
        Common = 12, // Common - this is (Q)ByteArray
    } Type; // KAK B Qt!!!
    
    typedef bool Bool_t;
    typedef int Int_t;
    typedef unsigned int UInt_t;
    typedef long long LongLong_t;
    typedef unsigned long long ULongLong_t;
    typedef double Double_t;
    typedef long Long_t;
    typedef short Short_t;
    typedef char Char_t;
    typedef unsigned long ULong_t;
    typedef unsigned short UShort_t;
    typedef unsigned char UChar_t;    
    typedef float Float_t;
    typedef signed char SChar_t;
    typedef string String_t;
    
    struct Description
    {
        unsigned char id;
        unsigned char flags;
        unsigned char rType; // Type
        unsigned char wType;
        unsigned char readSize;
        unsigned char writeSize;
        string name;

        void read(ByteArray &ba)
        {
            ba.append(id);
            ba.append(flags);
            ba.append(rType);
            ba.append(wType);
            ba.append(readSize);
            ba.append(writeSize);
            ba.append(name.data(), name.size());
        }

        Description() : id(0), flags(0), rType(0), wType(0), readSize(0), writeSize(0) {}
        Description(const ByteArray &ba)
        {
            id = ba[0];
            flags = ba[1] & ~(Read | Write); // read-write naoborot
            if (ba[1] & Read)
                flags |= Write;
            if (ba[1] & Write)
                flags |= Read;
            rType = ba[3];
            wType = ba[2];
            readSize = ba[5];
            writeSize = ba[4]; // specialno naoborot read i write, ibo master readit chto node writit
            name = string(ba.data()+6, ba.size()-6);
        }
    };

private:
    void *mReadPtr, *mWritePtr;
    int mAutoPeriod, mAutoTime; // automatic transmission period
    Description mDesc;
    bool mIsDevice;

    friend class ObjnetNode;
    friend class ObjnetMaster;
    friend class ObjnetDevice;

public:
    ObjectInfo();
    
    // vars binding:
    template<typename T>
    ObjectInfo(string name, T &var, Flags flags=ReadWrite);
    template<typename Tr, typename Tw>
    ObjectInfo(string name, Tr &varRead, Tw &varWrite, Flags flags=ReadWrite);

    // methods binding:
    template<class R>
    ObjectInfo(string name, Closure<R(void)> event, Flags flags=Read);
    template<class P0>
    ObjectInfo(string name, Closure<void(P0)> event, Flags flags=Write);
    template<class R, class P0>
    ObjectInfo(string name, Closure<R(P0)> event, Flags flags=ReadWrite);

    ByteArray read();
    bool write(const ByteArray &ba);
    ByteArray invoke(const ByteArray &ba);

    _String name() const {return _toString(mDesc.name);}
    Type rType() const {return static_cast<Type>(mDesc.rType);}
    Type wType() const {return static_cast<Type>(mDesc.wType);}
    Flags flags() const {return static_cast<Flags>(mDesc.flags);}
    
    inline bool isVolatile() const {return mDesc.flags & Volatile;}
    inline bool isReadable() const {return mDesc.flags & Read;}
    inline bool isWritable() const {return mDesc.flags & Write;}
    inline bool isStorable() const {return mDesc.flags & Save;}
    inline bool isHidden() const {return mDesc.flags & Hidden;}
    inline bool isDual() const {return mDesc.flags & Dual;}
    inline bool isInvokable() const {return mDesc.flags & Function;}

    #ifndef __ICCARM__
    QVariant toVariant();
    bool fromVariant(QVariant &v);
    #endif
};

template<typename T> static ObjectInfo::Type typeOfVar(T &var) {(void)var; return ObjectInfo::Common;}
#define DeclareTypeOfVar(Tp) template<> ObjectInfo::Type typeOfVar<ObjectInfo::Tp##_t>(ObjectInfo::Tp##_t &var) {(void)var; return ObjectInfo::Tp;}
//DeclareTypeOfVar(void, Void)
DeclareTypeOfVar(Bool)
DeclareTypeOfVar(Int)
DeclareTypeOfVar(UInt)
DeclareTypeOfVar(LongLong)
DeclareTypeOfVar(ULongLong)
DeclareTypeOfVar(Double)
DeclareTypeOfVar(Long)
DeclareTypeOfVar(Short)
DeclareTypeOfVar(Char)
DeclareTypeOfVar(ULong)
DeclareTypeOfVar(UShort)
DeclareTypeOfVar(UChar)
DeclareTypeOfVar(Float)
DeclareTypeOfVar(SChar)
template<> ObjectInfo::Type typeOfVar<_String>(_String &var) {(void)var; return ObjectInfo::String;}

template<typename T>
ObjectInfo::ObjectInfo(string name, T &var, Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0),
    mIsDevice(false)
{
    size_t sz = sizeof(T);
    Type t = typeOfVar(var);
    if (flags & Read)
    {
        mReadPtr = &var;
        mDesc.readSize = sz;
        mDesc.rType = t;
    }
    if (flags & Write)
    {
        mWritePtr = &var;
        mDesc.writeSize = sz;
        mDesc.wType = t;
    }
    mDesc.flags = flags;
    mDesc.name = name;
}

template<typename Tr, typename Tw>
ObjectInfo::ObjectInfo(string name, Tr &varRead, Tw &varWrite, Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0),
    mIsDevice(false)
{
    if (flags & Read)
    {
        mReadPtr = &varRead;
        mDesc.readSize = sizeof(Tr);
        mDesc.rType = typeOfVar(varRead);
    }
    if (flags & Write)
    {
        mWritePtr = &varWrite;
        mDesc.writeSize = sizeof(Tw);
        mDesc.wType = typeOfVar(varWrite);
    }
    mDesc.flags = flags | Dual;
    mDesc.name = name;
}


template<class R>
ObjectInfo::ObjectInfo(string name, Closure<R(void)> event, ObjectInfo::Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0),
    mIsDevice(false)
{
    mDesc.readSize = sizeof(R);
    mDesc.writeSize = 0; // no param
        
    if (mDesc.readSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Read);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<R(void)>*>(&mReadPtr) = event;
    
    R var;
    mDesc.rType = typeOfVar(var); // return type
    mDesc.wType = Void; // param type
    mDesc.flags = (flags | Function) & ~(Save | Write);
    mDesc.name = name;
}

template<> ObjectInfo::ObjectInfo<void>(string name, Closure<void(void)> event, ObjectInfo::Flags flags);

template<class P0>
ObjectInfo::ObjectInfo(string name, Closure<void(P0)> event, ObjectInfo::Flags flags) :
    mAutoPeriod(0), mAutoTime(0),
    mIsDevice(false)
{
    mDesc.readSize = 0;
    mDesc.writeSize = sizeof(P0); // no param
        
    if (mDesc.writeSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Write);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<void(P0)>*>(&mReadPtr) = event;
    
    P0 param;
    mDesc.rType = Void; // return type;
    mDesc.wType = typeOfVar(param); // param type
    mDesc.flags = (flags | Function) & ~(Save | Read);
    mDesc.name = name;
}

template<class R, class P0>
ObjectInfo::ObjectInfo(string name, Closure<R(P0)> event, ObjectInfo::Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0),
    mIsDevice(false)
{
    mDesc.readSize = sizeof(R);
    mDesc.writeSize = sizeof(P0); // no param
        
    if (mDesc.readSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Read);
    if (mDesc.writeSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Write);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<R(P0)>*>(&mReadPtr) = event;
    
    R ret;
    P0 param;
    mDesc.rType = typeOfVar(ret); // return type;
    mDesc.wType = typeOfVar(param); // param type
    mDesc.flags = (flags | Function) & ~(Save);
    mDesc.name = name;
}

}

#endif // OBJECTINFO_H
