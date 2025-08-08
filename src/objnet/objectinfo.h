#ifndef OBJECTINFO_H
#define OBJECTINFO_H

//#if __cplusplus > 199711L
//#include <type_traits>
//#endif

#include "objnetcommon.h"
#include "core/ringbuffer.h"

#ifndef QT_CORE_LIB
#include "core/quaternion.h"
#define QQuaternion Quaternion
#define _String string
#define _toString(x) (x)
#define _fromString(x) (x)
#else
#define _String QString
#define _toString(x) QString::fromStdString(x)
#define _fromString(x) (x).toStdString()
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
        Array   = 0x80, // object is array

        Constant    = Read,
        ReadOnly    = Read,
        Measurement = Read | Volatile,
        WriteOnly   = Write,
        Control     = Write | Volatile,
        ReadWrite   = Read | Write,
        Exchange    = ReadWrite | Volatile,
        SecretVar   = ReadWrite | Hidden,
        Storage     = ReadWrite | Save,
        SecretSetting = Storage | Hidden,
        HiddenMeasurement = Measurement | Hidden
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
        
        // Qt-specific types
        QTransform = 80,
        QMatrix4x4 = 81,
        QVector2D = 82,
        QVector3D = 83,
        QVector4D = 84,
        QQuaternion = 85,

        String = 10,  // QString B Qt, string B APMe
        StringList = 11, // QStringList | std::vector<std::string>
        Common = 12, // Common - this is (Q)ByteArray
        
        Compound = 0x80, // bits 0...6 reflect subobject count
    } Type; // KAK B Qt!!!

    typedef void Void_t;
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
    typedef vector<string> StringList_t;
    typedef ::QVector3D QVector3D_t;
    typedef ::QQuaternion QQuaternion_t;

    struct Description
    {
        unsigned char id;
        unsigned char flags;
        unsigned char rType; // Type
        unsigned char wType;
        unsigned char readSize;
        unsigned char writeSize;
        string name;

        void read(ByteArray &ba) const
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
    void *mReadPtr;
    void *mWritePtr;
    int mAutoPeriod, mAutoTime; // automatic transmission period
    unsigned char mAutoReceiverAddr = 0; // address of receiver for automatic transmission
    bool mTimedRequest; // если синхронизованный объект
    Description mDesc;
    bool mIsDevice;
    bool mValid;
    static int mAssignId;
    uint8_t mArrayItemCount = 1;
    
    ObjectInfo *m_parentObject;
    std::vector<ObjectInfo> m_subobjects;

    friend class ObjnetNode;
    friend class ObjnetMaster;
    friend class ObjnetDevice;
    friend class ObjnetStorage;

    static constexpr int sizeofType(Type type);
    
    uint8_t *nextReadPtr() const;
    uint8_t *nextWritePtr() const;

public:
    ObjectInfo();

    // vars binding:
//#if __cplusplus > 199711L
//    template<typename T, typename std::enable_if<!std::is_class<T>::value, bool>::type = true>
//    ObjectInfo(string name, T &var, Flags flags=ReadWrite);    
//    template<typename T, typename std::enable_if<!std::is_class<T>::value, bool>::type = true>
//    ObjectInfo(string name, const T &var, Flags flags=ReadOnly);
//    template<typename Tr, typename Tw>
//    ObjectInfo(string name, const Tr &varRead, Tw &varWrite, Flags flags=ReadWrite);
//#else
    template<typename T>
    ObjectInfo(string name, T &var, Flags flags=ReadWrite);    
    template<typename T>
    ObjectInfo(string name, const T &var, Flags flags=ReadOnly);
    template<typename Tr, typename Tw>
    ObjectInfo(string name, const Tr &varRead, Tw &varWrite, Flags flags=ReadWrite);
//#endif

    // array binding:
    template<typename T, int N>
    ObjectInfo(string name, T (&var)[N], Flags flags=ReadWrite);
    template<typename T, int N>
    ObjectInfo(string name, const T (&var)[N], Flags flags=ReadWrite);
    
//#if __cplusplus > 199711L
//    // struct binding:
//    template<typename T, typename std::enable_if<std::is_class<T>::value, bool>::type = true>
//    ObjectInfo(string name, T &var, Flags flags=ReadWrite);   
//#endif
    template<typename T>
    ObjectInfo &field(string name);
    
    ObjectInfo &group(string name); // for substructures
    ObjectInfo &endGroup();

    // methods binding:
    template<class R>
    ObjectInfo(string name, Closure<R(void)> event, Flags flags=Read);
    template<class P0>
    ObjectInfo(string name, Closure<void(P0)> event, Flags flags=Write);
    template<class R, class P0>
    ObjectInfo(string name, Closure<R(P0)> event, Flags flags=ReadWrite);
    
    // buffer binding:
    template<typename T>
    ObjectInfo(string name, RingBuffer<T> &buffer, Flags flags=ReadWrite);

    ByteArray read();
    bool write(const ByteArray &ba);
    ByteArray invoke(const ByteArray &ba);

    _String name() const {return _toString(mDesc.name);}
    Type rType() const {return static_cast<Type>(mDesc.rType);}
    Type wType() const {return static_cast<Type>(mDesc.wType);}
    Flags flags() const {return static_cast<Flags>(mDesc.flags);}
    int readSize() const {return mDesc.readSize;}
    int writeSize() const {return mDesc.writeSize;}

    bool isValid() const;
    
    inline bool isVolatile() const {return mDesc.flags & Volatile;}
    inline bool isReadable() const {return mDesc.flags & Read;}
    inline bool isWritable() const {return mDesc.flags & Write;}
    inline bool isStorable() const {return mDesc.flags & Save;}
    inline bool isHidden() const {return mDesc.flags & Hidden;}
    inline bool isDual() const {return mDesc.flags & Dual;}
    inline bool isInvokable() const {return mDesc.flags & Function;}
    inline bool isArray() const {return mDesc.flags & Array;}
    inline bool isCompound() const {return mDesc.rType >= Compound;}
    inline bool isBuffer() const {return !mDesc.writeSize && !mDesc.readSize && (mDesc.flags & Array);}

    inline int wCount() const
    {
        int sz = sizeofType((Type)mDesc.wType);
        if (isArray() && mDesc.wType == String)
            return mDesc.writeSize;
        return (isArray() && sz)? mDesc.writeSize / sz: 1;
    }
    inline int rCount() const
    {
        int sz = sizeofType((Type)mDesc.rType);
        if (isArray() && mDesc.rType == String)
            return mDesc.readSize;
        return (isArray() && sz)? mDesc.readSize / sz: 1;
    }

    inline const Description &description() {return mDesc;}
    inline uint8_t id() const {return mDesc.id;}
    
    Closure<void(unsigned char)> onValueChanged;

    #ifdef QT_CORE_LIB
    QVariant toVariant();
    bool fromVariant(QVariant &v);
    #endif
    
    ByteArray toString();
    bool fromString(const ByteArray &s);
    
    ObjectInfo &subobject(uint8_t idx);
    uint8_t subobjectCount() const {return m_subobjects.size();}
    ObjectInfo *parentObject() {return m_parentObject;}
    
    template <typename T>
    static constexpr Type typeValue() {return Common;} // by default
};

#define DeclareTypeValueSpec(T) \
    template <> constexpr ObjectInfo::Type ObjectInfo::typeValue<ObjectInfo::T##_t>() \
    {return ObjectInfo::T;}
    
DeclareTypeValueSpec(Void)
DeclareTypeValueSpec(Bool)
DeclareTypeValueSpec(Int)
DeclareTypeValueSpec(UInt)
DeclareTypeValueSpec(LongLong)
DeclareTypeValueSpec(ULongLong)
DeclareTypeValueSpec(Double)
DeclareTypeValueSpec(Long)
DeclareTypeValueSpec(Short)
DeclareTypeValueSpec(Char)
DeclareTypeValueSpec(ULong)
DeclareTypeValueSpec(UShort)
DeclareTypeValueSpec(UChar)
DeclareTypeValueSpec(Float)
DeclareTypeValueSpec(SChar)
template <> constexpr ObjectInfo::Type ObjectInfo::typeValue<_String>() {return ObjectInfo::String;}
DeclareTypeValueSpec(StringList)
DeclareTypeValueSpec(QVector3D)
DeclareTypeValueSpec(QQuaternion)

constexpr int ObjectInfo::sizeofType(ObjectInfo::Type type)
{
    switch (type)
    {
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
    }
}

//#if __cplusplus > 199711L
//template<typename T, typename std::enable_if<!std::is_class<T>::value, bool>::type>
//#else
template<typename T>
//#endif
ObjectInfo::ObjectInfo(string name, T &var, Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    size_t sz = sizeof(T);
    Type t = typeValue<T>();
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
    mDesc.id = mAssignId++;
}

template<> ObjectInfo::ObjectInfo(string name, ByteArray &var, Flags flags);

//#if __cplusplus > 199711L
//template<typename T, typename std::enable_if<!std::is_class<T>::value, bool>::type>
//#else
template<typename T>
//#endif
ObjectInfo::ObjectInfo(string name, const T &var, Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    flags = static_cast<Flags>(flags & (~Write));
    flags = static_cast<Flags>(flags & (~Save));
    size_t sz = sizeof(T);
    Type t = typeValue<T>();
    if (flags & Read)
    {
        mReadPtr = const_cast<T*>(&var);
        mDesc.readSize = sz;
        mDesc.rType = t;
    }
    mDesc.flags = flags;
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

template<typename Tr, typename Tw>
ObjectInfo::ObjectInfo(string name, const Tr &varRead, Tw &varWrite, Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    flags = static_cast<Flags>(flags & (~Save));
    if (flags & Read)
    {
        mReadPtr = &varRead;
        mDesc.readSize = sizeof(Tr);
        mDesc.rType = typeValue<Tr>();
    }
    if (flags & Write)
    {
        mWritePtr = &varWrite;
        mDesc.writeSize = sizeof(Tw);
        mDesc.wType = typeValue<Tw>();
    }
    mDesc.flags = flags | Dual;
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

template<typename T, int N>
ObjectInfo::ObjectInfo(string name, T (&var)[N], Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    Type t = typeValue<T>();
    size_t sz = sizeof(T) * N;
    mArrayItemCount = N;
//    if (sz > 255)
//        sz = 0; // can't :c
    if (t == String)
        sz = N;
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
    mDesc.flags = flags | Array;
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

template<typename T, int N>
ObjectInfo::ObjectInfo(string name, const T (&var)[N], Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    Type t = typeValue<T>();
    size_t sz = sizeof(T) * N;
    mArrayItemCount = N;
//    if (sz > 255)
//        sz = 0; // can't :c
    if (t == String)
        sz = N;
    if (flags & Read)
    {
        mReadPtr = &var;
        mDesc.readSize = sz;
        mDesc.rType = t;
    }
    mDesc.flags = (flags & ~(Write | Save)) | Array;
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

//#if __cplusplus > 199711L
//template<typename T, typename std::enable_if<std::is_class<T>::value, bool>::type>
//ObjectInfo::ObjectInfo(string name, T &var, Flags flags) :
//    mReadPtr(0), mWritePtr(0),
//    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
//    mIsDevice(false),
//    mValid(true),
//    m_parentObject(0L)
//{
//    size_t sz = sizeof(T);
//    Type t = typeValue<T>();
//    if (flags & Read)
//    {
//        mReadPtr = &var;
//        mDesc.readSize = sz;
//        mDesc.rType = t;
//    }
//    if (flags & Write)
//    {
//        mWritePtr = &var;
//        mDesc.writeSize = sz;
//        mDesc.wType = t;
//    }
//    mDesc.flags = flags;
//    mDesc.name = name;
//    mDesc.id = mAssignId++;
//}
//#endif

template<typename T>
ObjectInfo &ObjectInfo::field(string name)
{
    // if this is the first field, clear size
//    if (!subobjectCount())
//        mDesc.readSize = mDesc.writeSize = 0;
    
    if (isArray())
        mDesc.readSize = mDesc.writeSize = mArrayItemCount;
    
    ObjectInfo info;
    m_subobjects.push_back(info);
    ObjectInfo &obj = m_subobjects.back();
    ObjectInfo *prev = nullptr;
    if (m_subobjects.size() > 1)
        prev = &m_subobjects[m_subobjects.size() - 2];
    // bugfix:
    for (ObjectInfo &o: m_subobjects)
        o.m_parentObject = this;
    /// @todo Test the fix of the bug with m_parentObject!
//    obj.m_parentObject = this;
    size_t sz = sizeof(T);
//    T var;
    Type t = typeValue<T>();
    if (t == String)
        sz = 0;
    if (mDesc.flags & Read)
    {
        uint8_t *ptr = (uint8_t *)mReadPtr;
        if (prev)
            ptr = prev->nextReadPtr();
        obj.mReadPtr = ptr;
        obj.mDesc.readSize = sz;
        obj.mDesc.rType = t;
    }
    if (mDesc.flags & Write)
    {
        uint8_t *ptr = (uint8_t *)mWritePtr;
        if (prev)
            ptr = prev->nextWritePtr();
        obj.mWritePtr = ptr;
        obj.mDesc.writeSize = sz;
        obj.mDesc.wType = t;
    }
    obj.mDesc.flags = mDesc.flags;
    obj.mDesc.name = name;
    obj.mDesc.id = m_subobjects.size() - 1;
    
    if (!sz)
    {
        for (ObjectInfo *o = this; o; o = o->m_parentObject)
        {
            o->mDesc.readSize = 0;// += obj.mDesc.readSize;
            o->mDesc.writeSize = 0;//+= obj.mDesc.writeSize;
        }
    }
    
    mDesc.rType = (uint8_t)Compound + m_subobjects.size();
    mDesc.wType = (uint8_t)Compound + m_subobjects.size();
    return *this;
}

//template<typename T, int N>
//ObjectInfo &ObjectInfo::array(string name)
//{  
//    ObjectInfo info;
//    m_subobjects.push_back(info);
//    ObjectInfo &obj = m_subobjects.back();
//    // bugfix:
//    for (ObjectInfo &o: m_subobjects)
//        o->m_parentObject = this;
//    /// @todo Test the fix of the bug with m_parentObject!
////    obj.m_parentObject = this;
//    size_t sz = sizeof(T);
//    T var;
//    Type t = typeValue<T>();
//    if (t == String)
//        sz = 0;
//    if (mDesc.flags & Read)
//    {
//        obj.mReadPtr = (uint8_t*)mReadPtr + mDesc.readSize;
//        obj.mDesc.readSize = sz? sz * N: N;
//        obj.mDesc.rType = t;
//    }
//    if (mDesc.flags & Write)
//    {
//        obj.mWritePtr = (uint8_t*)mWritePtr + mDesc.writeSize;
//        obj.mDesc.writeSize = sz? sz * N: N;
//        obj.mDesc.wType = t;
//    }
//    obj.mDesc.flags = mDesc.flags | Array;
//    obj.mDesc.name = name;
//    obj.mDesc.id = m_subobjects.size() - 1;
//
//    for (ObjectInfo *o = this; o; o = o->m_parentObject)
//    {
//        o->mDesc.readSize = 0;// += obj.mDesc.readSize;
//        o->mDesc.writeSize = 0;//+= obj.mDesc.writeSize;
//    }
//    
//    mDesc.rType = (uint8_t)Compound + m_subobjects.size();
//    mDesc.wType = (uint8_t)Compound + m_subobjects.size();
//    return *this;
//}

template<class R>
ObjectInfo::ObjectInfo(string name, Closure<R(void)> event, ObjectInfo::Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    mDesc.readSize = sizeof(R);
    mDesc.writeSize = 0; // no param

    if (mDesc.readSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Read);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<R(void)>*>(&mReadPtr) = event;

//    R var;
    mDesc.rType = typeValue<R>(); // return type
    mDesc.wType = Void; // param type
    mDesc.flags = (flags | Function) & ~(Save | Write);
    mDesc.name = name;
    mDesc.id = mAssignId++;
}
   
#ifndef QT_CORE_LIB
template<> ObjectInfo::ObjectInfo<void>(string name, Closure<void(void)> event, ObjectInfo::Flags flags);
template<> ObjectInfo::ObjectInfo(string name, Closure<ByteArray(void)> event, ObjectInfo::Flags flags);
template<> ObjectInfo::ObjectInfo(string name, Closure<void(ByteArray)> event, ObjectInfo::Flags flags);
template<> ObjectInfo::ObjectInfo(string name, Closure<ByteArray(ByteArray)> event, ObjectInfo::Flags flags);
#endif

template<class P0>
ObjectInfo::ObjectInfo(string name, Closure<void(P0)> event, ObjectInfo::Flags flags) :
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    mDesc.readSize = 0;
    mDesc.writeSize = sizeof(P0); // no param

    if (mDesc.writeSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Write);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<void(P0)>*>(&mReadPtr) = event;

//    P0 param;
    mDesc.rType = Void; // return type;
    mDesc.wType = typeValue<P0>(); // param type
    mDesc.flags = (flags | Function) & ~(Save | Read);
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

template<class R, class P0>
ObjectInfo::ObjectInfo(string name, Closure<R(P0)> event, ObjectInfo::Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    mDesc.readSize = sizeof(R);
    mDesc.writeSize = sizeof(P0); // no param

    if (mDesc.readSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Read);
    if (mDesc.writeSize)
        flags = static_cast<ObjectInfo::Flags>(flags | Write);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<R(P0)>*>(&mReadPtr) = event;

//    R ret;
//    P0 param;
    mDesc.rType = typeValue<R>(); // return type;
    mDesc.wType = typeValue<P0>(); // param type
    mDesc.flags = (flags | Function) & ~(Save);
    mDesc.name = name;
    mDesc.id = mAssignId++;
}


template<typename T>
ObjectInfo::ObjectInfo(string name, RingBuffer<T> &buffer, Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    mDesc.readSize = 0;
    mDesc.writeSize = 0;
    
    mReadPtr = &buffer;
    mWritePtr = &buffer;
    
//    T var;
    if (flags & Read)
        mDesc.rType = typeValue<T>();
    if (flags & Write)
        mDesc.wType = typeValue<T>();
    mDesc.flags = (flags | Array) & ~Save;
    mDesc.name = name;
    mDesc.id = mAssignId++;
}



}

#endif // OBJECTINFO_H
