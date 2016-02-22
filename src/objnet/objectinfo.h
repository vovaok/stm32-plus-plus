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
        unsigned char type; // Type
        unsigned char flags;
        unsigned char readSize;
        unsigned char writeSize;
        string name;

        void read(ByteArray &ba)
        {
            ba.append(id);
            ba.append(type);
            ba.append(flags);
            ba.append(readSize);
            ba.append(writeSize);
            ba.append(name.data(), name.size());
        }

        Description() : id(0), type(0), flags(0), readSize(0), writeSize(0) {}
        Description(const ByteArray &ba)
        {
            id = ba[0];
            type = ba[1];
            flags = ba[2] & ~(Read | Write); // read-write naoborot
            if (ba[2] & Read)
                flags |= Write;
            if (ba[2] & Write)
                flags |= Read;
            readSize = ba[4];
            writeSize = ba[3]; // specialno naoborot read i write, ibo master readit chto node writit
            name = string(ba.data()+5, ba.size()-5);
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

//    ObjectInfo &bindVariableRO(string name, void *ptr, size_t size);
//    ObjectInfo &bindVariableWO(string name, void *ptr, size_t size);
//    ObjectInfo &bindVariableRW(string name, void *ptr, size_t size);
//    ObjectInfo &bindVariableInOut(string name, void *inPtr, size_t inSize, void *outPtr, size_t outSize);
//    ObjectInfo &bindString(string name, string *str);

    ByteArray read();
    bool write(const ByteArray &ba);

    _String name() const {return _toString(mDesc.name);}
    Type type() const {return static_cast<Type>(mDesc.type);}
//    _String typeName() const {return ...;}
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
//        if (typeid(T) == typeid(string))
//            sz = 1;
    if (flags & Read)
    {
        mReadPtr = &var;
        mDesc.readSize = sz;
    }
    if (flags & Write)
    {
        mWritePtr = &var;
        mDesc.writeSize = sz;
    }

    mDesc.type = typeOfVar(var);
    mDesc.flags = flags;
    mDesc.name = name;
}

}

#endif // OBJECTINFO_H
