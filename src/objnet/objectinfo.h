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
    
    //template<typename T> static Type typeOfVar(T &var);

private:
    void *mReadPtr, *mWritePtr;
    Description mDesc;

    friend class ObjnetNode;
    friend class ObjnetMaster;
    friend class ObjnetDevice;

public:
    ObjectInfo();
    
    template<typename T>
    ObjectInfo(string name, T &var, Flags flags=ReadWrite);
    template<typename Tr, typename Tw>
    ObjectInfo(string name, Tr &varRead, Tw &varWrite, Flags flags=ReadWrite);
    
//    ObjectInfo(string name, NotifyEvent var, Flags flags=ReadWrite);

    ByteArray read();
    bool write(const ByteArray &ba);

    _String name() const {return _toString(mDesc.name);}
    Type rType() const {return static_cast<Type>(mDesc.rType);}
    Type wType() const {return static_cast<Type>(mDesc.wType);}
    Flags flags() const {return static_cast<Flags>(mDesc.flags);}

    #ifndef __ICCARM__
    QVariant toVariant();
    bool fromVariant(QVariant &v);
    #endif
};

template<typename T> static ObjectInfo::Type typeOfVar(T &var) {(void)var; return ObjectInfo::Common;}
#define DeclareTypeOfVar(T, Tp) template<> ObjectInfo::Type typeOfVar<T>(T &var) {(void)var; return ObjectInfo::Tp;}
//DeclareTypeOfVar(void, Void)
DeclareTypeOfVar(bool, Bool)
DeclareTypeOfVar(int, Int)
DeclareTypeOfVar(unsigned int, UInt)
DeclareTypeOfVar(long long, LongLong)
DeclareTypeOfVar(unsigned long long, ULongLong)
DeclareTypeOfVar(double, Double)
DeclareTypeOfVar(long, Long)
DeclareTypeOfVar(short, Short)
DeclareTypeOfVar(char, Char)
DeclareTypeOfVar(unsigned long, ULong)
DeclareTypeOfVar(unsigned short, UShort)
DeclareTypeOfVar(unsigned char, UChar)
DeclareTypeOfVar(float, Float)
DeclareTypeOfVar(signed char, SChar)
DeclareTypeOfVar(_String, String)

template<typename T>
ObjectInfo::ObjectInfo(string name, T &var, Flags flags) :
    mReadPtr(0), mWritePtr(0)
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
    mReadPtr(0), mWritePtr(0)
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

}

#endif // OBJECTINFO_H
