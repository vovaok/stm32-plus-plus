#include "objectinfo.h"

using namespace Objnet;

int ObjectInfo::mAssignId = 0;

ObjectInfo::ObjectInfo() :
    mReadPtr(0L), mWritePtr(0L),
    mAutoPeriod(0), mAutoTime(0),
    mTimedRequest(false),
    mIsDevice(false),
    mValid(false),
    m_parentObject(0L)
{
}

template<> ObjectInfo::ObjectInfo(string name, ByteArray &var, Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    if (flags & Read)
    {
        mReadPtr = &var;
        mDesc.readSize = 0;
        mDesc.rType = Common;
    }
    if (flags & Write)
    {
        mWritePtr = &var;
        mDesc.writeSize = 0;
        mDesc.wType = Common;
    }
    mDesc.flags = flags;
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

#ifndef QT_CORE_LIB
template<> ObjectInfo::ObjectInfo<void>(string name, Closure<void(void)> event, ObjectInfo::Flags flags) :
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mTimedRequest(false),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
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

template<>
ObjectInfo::ObjectInfo(string name, Closure<ByteArray(void)> event, ObjectInfo::Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    mDesc.readSize = 0; // pure ByteArray
    mDesc.writeSize = 0; // no param

    flags = static_cast<ObjectInfo::Flags>(flags | Read);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<ByteArray(void)>*>(&mReadPtr) = event;

    mDesc.rType = Common; // return type
    mDesc.wType = Void; // param type
    mDesc.flags = (flags | Function) & ~(Save | Write);
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

template<>
ObjectInfo::ObjectInfo(string name, Closure<void(ByteArray)> event, ObjectInfo::Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    mDesc.readSize = 0; // no return
    mDesc.writeSize = 0; // pure ByteArray

    flags = static_cast<ObjectInfo::Flags>(flags | Write);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<void(ByteArray)>*>(&mReadPtr) = event;

    mDesc.rType = Void; // return type
    mDesc.wType = Common; // param type
    mDesc.flags = (flags | Function) & ~(Save | Read);
    mDesc.name = name;
    mDesc.id = mAssignId++;
}

template<>
ObjectInfo::ObjectInfo(string name, Closure<ByteArray(ByteArray)> event, ObjectInfo::Flags flags) :
    mReadPtr(0), mWritePtr(0),
    mAutoPeriod(0), mAutoTime(0), mAutoReceiverAddr(0),
    mIsDevice(false),
    mValid(true),
    m_parentObject(0L)
{
    mDesc.readSize = 0; // pure ByteArray
    mDesc.writeSize = 0; // pure ByteArray

    flags = static_cast<ObjectInfo::Flags>(flags | Read | Write);

    // mReadPtr:mWritePtr contains variable of Closure<> type, not the pointer!!!
    *reinterpret_cast<Closure<ByteArray(ByteArray)>*>(&mReadPtr) = event;

    mDesc.rType = Common; // return type
    mDesc.wType = Common; // param type
    mDesc.flags = (flags | Function) & ~(Save);
    mDesc.name = name;
    mDesc.id = mAssignId++;
}
#endif
//---------------------------------------------------------

ObjectInfo &ObjectInfo::group(string name)
{
    ObjectInfo info;
    m_subobjects.push_back(info);
    ObjectInfo &obj = m_subobjects.back();
    obj.m_parentObject = this;
    if (mDesc.flags & Read)
    {
//        obj.mReadPtr = &var;
        obj.mDesc.readSize = 0;
        obj.mDesc.rType = Compound;
    }
    if (mDesc.flags & Write)
    {
//        obj.mWritePtr = &var;
        obj.mDesc.writeSize = 0;
        obj.mDesc.wType = Compound;
    }
    obj.mDesc.flags = mDesc.flags;
    obj.mDesc.name = name;
    obj.mDesc.id = m_subobjects.size() - 1;
    mDesc.rType = (uint8_t)Compound + m_subobjects.size();
    mDesc.wType = (uint8_t)Compound + m_subobjects.size();
    return obj;
}

ObjectInfo &ObjectInfo::endGroup()
{
    return *m_parentObject;
}

bool ObjectInfo::isValid() const
{
    bool result = mValid;
    for (int i=0; i<m_subobjects.size(); i++)
        result = result && m_subobjects[i].isValid();
    return result;
}

ByteArray ObjectInfo::read()
{
    if ((mDesc.flags & Function) && !mIsDevice)
        return invoke(ByteArray());
    
    if (!mReadPtr || !(mDesc.flags & Read))
        return ByteArray();
    if (mDesc.rType == String)
    {
        const _String *str = reinterpret_cast<const _String*>(mReadPtr);
        if (str)
        {
            ByteArray ba;
            int N = 1;
            if (isArray())
                N = mDesc.writeSize;
            for (int i=0; i<N; i++)
            {
                if (i)
                    ba.append('\0');
                string s = _fromString(str[i]);
                ba.append(s.c_str());
            }
            return ba;
        }
        return ByteArray();
    }
    else if (mDesc.rType == StringList)
    {
        ByteArray ba;
        const vector<string> *v = reinterpret_cast<const vector<string> *>(mReadPtr);
        if (v)
        {
            
            for (const string &s: *v)
            {
                ba.append(s.c_str());
                ba.append('\0');
            }
            ba.chop(1);
        }
        return ba;
    }
    else if (mDesc.rType == Common && mDesc.readSize == 0) // pure (Q)ByteArray
    {
        return *reinterpret_cast<const ByteArray*>(mReadPtr);
    }
    else if (!mDesc.readSize && isArray()) // this is RingBuffer
    {
        ByteArray ba;
        if (mDesc.rType == Float)
        {
            RingBuffer<float> *ring = reinterpret_cast<RingBuffer<float> *>(mWritePtr);
            int sz = ring->size();
            ba.resize(sz * sizeof(float));
            float *dst = reinterpret_cast<float *>(ba.data());
//            while (!ring->isEmpty())
            for (; sz; --sz)
                *dst++ = ring->take_front();
//            int csz = ring->cont_size() * sizeof(float);
//            ba.append(reinterpret_cast<const char *>(&ring->front()), csz);
//            ba.append(reinterpret_cast<const char *>(ring->data()), sz - csz);
//            ring->clear();
        }
        else if (mDesc.rType == Int) 
        {
            /// @todo Implement other types in the ObjectInfo with RingBuffer
        }
        return ba;
    }
    else if ((!mDesc.readSize || isArray()) && isCompound()) // need to serialize compound object
    {
        ByteArray ba;
        int N = 1;
        if (isArray())
            N = mDesc.readSize;
        for (int i=0; i<N; i++)
        {
            for (ObjectInfo &o: m_subobjects)
            {
                ba.append(o.read());
                if (o.mDesc.rType == String)
                    ba.append('\0');
            }
        }
        return ba;
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
    
    bool changed = false;
  
    if (!mWritePtr || !(mDesc.flags & Write))
        return false;
    if (mDesc.wType == String)
    {
        _String *str = reinterpret_cast<_String*>(mWritePtr);
        if (!str)
            return false;
        else 
        {
            const char *src = ba.data();
            const char *end = src + ba.size();
            int N = 1;
            if (isArray())
                N = mDesc.writeSize;
            for (int i=0; i<N && src < end; i++)
            {
                _String newstr = _toString(string(src));
                if (onValueChanged && (str[i] != newstr))
                    changed = true;
                str[i] = newstr;
                src += newstr.length() + 1;
            }
        }
    }
    else if (mDesc.wType == StringList)
    {
        vector<string> *v = reinterpret_cast<vector<string> *>(mWritePtr);
        if (v)
        {
            vector<string> newv;
            int idx = 0;
            int sz = ba.size();
            while (idx < sz)
            {
                string s = string(ba.data() + idx);
                newv.push_back(s);
                idx += s.length() + 1;
            }
            if (onValueChanged && *v != newv)
                changed = true;
            *v = newv;
        }
    }
    else if (mDesc.wType == Common && mDesc.writeSize == 0) // pure (Q)ByteArray
    {
        ByteArray &oldBa = *reinterpret_cast<ByteArray*>(mWritePtr);
        if (onValueChanged && (oldBa != ba))
            changed = true;
        oldBa = ba;
    }
    else if (!mDesc.writeSize && isArray()) // this is RingBuffer
    {
        if (mDesc.wType == Float)
        {
            RingBuffer<float> *ring = reinterpret_cast<RingBuffer<float> *>(mWritePtr);
            const float *src = reinterpret_cast<const float *>(ba.data());
            int sz = ba.size() / sizeof(float);
            for (; sz; --sz)
                ring->push_back(*src++);
        }
        else if (mDesc.wType == Int)
        {
            /// @todo Implement other types in the ObjectInfo with RingBuffer
        }
    }
    else if ((!mDesc.writeSize || isArray()) && isCompound()) // need to deserialize compound object
    {
        int N = 1;
        if (isArray())
            N = mDesc.writeSize;
        const char *src = ba.data();
        const char *end = src + ba.size();
        for (int i=0; i<N; i++)
        {
            for (ObjectInfo &o: m_subobjects)
            {
                if (src >= end)
                    return false;
                
                int sz = o.mDesc.writeSize;
                if (o.mDesc.wType == String)
                {
                    /// @todo add support of string arrays OR REWRITE ALL OF THIS SHIT COMPLETELY!!
                    sz = strlen(src) + 1;
//                    if (src + sz < end)
//                        sz++; // terminating '\0'
                }
                
                if (sz)
                {
                    ByteArray bb = ByteArray::fromRawData(src, sz);
                    o.write(bb);
                    src += sz;
                }
                else
                    return false;                    
            }
        }
    }
    else if ((size_t)ba.size() == mDesc.writeSize)
    {
        unsigned char *dst = reinterpret_cast<unsigned char*>(mWritePtr);
        for (size_t i=0; i<mDesc.writeSize; i++)
        {
            unsigned char b = ba.at(i);
            if (onValueChanged && dst[i] != b)
            {
                changed = true;
            }
            dst[i] = b;
        }
    }
    else
    {
        return false;
    }
    
    if (onValueChanged && changed)
        onValueChanged(mDesc.id);
    return true;
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
                case Common:
                    if (!mDesc.writeSize)
                        (*reinterpret_cast<Closure<void(ByteArray)>*>(&mReadPtr))(ba);
                    break;
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
                case Common: \
                    if (!mDesc.writeSize) \
                      result = (*reinterpret_cast<Closure<Tr##_t(ByteArray)>*>(&mReadPtr))(ba); \
                    break; \
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
                case Common: \
                    if (!mDesc.writeSize) \
                      result = (*reinterpret_cast<Closure<string(ByteArray)>*>(&mReadPtr))(ba); \
                    break; \
            }
            ret.append(result.c_str(), result.length()); 
          } break;
//                    Common = 12,
        
        case Common:
          if (!mDesc.writeSize)
          {
            ByteArray result;
            switch (mDesc.wType)
            {
                case Void: result = (*reinterpret_cast<Closure<ByteArray(void)>*>(&mReadPtr))(); break;
                case Common: result = (*reinterpret_cast<Closure<ByteArray(ByteArray)>*>(&mReadPtr))(ba); break;
            }
            ret.append(result); 
          }          
          break;
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
    if (mDesc.wType == Common && mDesc.writeSize)
        return ByteArray(reinterpret_cast<const char*>(mWritePtr), mDesc.writeSize);
//    if (mDesc.type == String)
//        return *reinterpret_cast<_String*>(mWritePtr);

    if (isCompound())
    {
        QVariantMap comp;
        for (int i=0; i<m_subobjects.size(); i++)
        {
            QString name = m_subobjects[i].name();
            comp[name] = m_subobjects[i].toVariant();
        }
        return comp;
    }

    if (mDesc.flags & Array)
    {
        if (!mDesc.writeSize) // this is RingBuffer
        {
            QList<QVariant> vec;
            if (mDesc.wType == Float)
            {
                RingBuffer<float> *ring = reinterpret_cast<RingBuffer<float> *>(mWritePtr);
                while (!ring->isEmpty())
                    vec << ring->take_front();
            }
            else if (mDesc.wType == Int)
            {
              
            }
            return vec;
        }
      
        int sz = sizeofType((Type)mDesc.wType);
        if (sz || isArray())
        {
            int N = sz? mDesc.writeSize / sz: mDesc.writeSize;
            if (mDesc.wType == String)
                sz = sizeof(_String);
            QList<QVariant> vec;
            for (int i=0; i<N; i++)
                vec << QVariant(mDesc.wType, reinterpret_cast<const char*>(mWritePtr) + sz*i);
            return vec;
        }
        return QVariant();
    }

    if (mDesc.wType == StringList)
    {
        QStringList list;
        const vector<string> &v = *reinterpret_cast<const vector<string> *>(mWritePtr);
        for (const string &s: v)
            list << QString::fromStdString(s);
        return list;
    }
    return QVariant(mDesc.wType, mWritePtr);
}

bool ObjectInfo::fromVariant(QVariant &v)
{
    if (mDesc.rType == Common && mDesc.readSize)
    {
        ByteArray ba = v.toByteArray();
        int sz = ba.size();
        sz = sz > mDesc.readSize? mDesc.readSize: sz;
        for (int i=0; i<sz; i++)
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(mReadPtr))[i] = ba[i];
        return true;
    }
    else if (mDesc.rType == Common)
    {
        *const_cast<ByteArray*>(reinterpret_cast<const ByteArray*>(mReadPtr)) = *reinterpret_cast<ByteArray*>(v.data());
        return true;
    }

    if (isCompound())
    {
        bool ok = true;
        QVariantMap comp = v.toMap();
        for (int i=0; i<m_subobjects.size(); i++)
        {
            QString name = m_subobjects[i].name();
            ok = ok && m_subobjects[i].fromVariant(comp[name]);
        }
        return ok;
    }

    if (mDesc.flags & Array)
    {
        QVariantList list = v.toList();
        
        if (!mDesc.readSize) // this is RingBuffer
        {
            QList<QVariant> vec;
            if (mDesc.rType == Float)
            {
                RingBuffer<float> *ring = reinterpret_cast<RingBuffer<float> *>(mWritePtr);
                for (QVariant &v: list)
                    ring->push_back(v.toFloat());
                return true;
            }
            else if (mDesc.wType == Int)
            {
              
            }
            return false;
        }
      
        int sz = sizeofType((Type)mDesc.rType);
        int N = sz? mDesc.readSize / sz: mDesc.readSize;
        for (int j=0; j<N && j<list.size(); j++)
        {
            QVariant v = list[j];
            if (mDesc.rType != v.type())
                return false;

            if (mDesc.rType == String)
            {
                _String *str = const_cast<_String *>(reinterpret_cast<const _String *>(mReadPtr));
                str[j] = v.toString();
            }
            else for (int i=0; i<sz; i++)
                const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(mReadPtr))[i+j*sz] = reinterpret_cast<unsigned char*>(v.data())[i];
        }
        return true;
    }
    else if (mDesc.rType == v.type())
    {
        if (mDesc.rType == String)
            *const_cast<_String *>(reinterpret_cast<const _String *>(mReadPtr)) = v.toString();
        else if (mDesc.wType == StringList)
        {
            QStringList list = v.toStringList();
            StringList_t vec;
            for (const QString &s: list)
                vec.push_back(s.toStdString());
            *const_cast<StringList_t *>(reinterpret_cast<const StringList_t *>(mReadPtr)) = vec;
        }
        else for (int i=0; i<mDesc.readSize; i++)
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(mReadPtr))[i] = reinterpret_cast<unsigned char*>(v.data())[i];
        return true;
    }
    return false;
}
#endif
//---------------------------------------------------------

ObjectInfo &ObjectInfo::subobject(uint8_t idx)
{
    if (idx < m_subobjects.size())
        return m_subobjects[idx];
    return *this;
}
              
uint8_t *ObjectInfo::nextReadPtr() const
{
    int sz = mDesc.readSize;
    if (mDesc.rType == String)
    {
        sz = sizeof(_String);
        if (isArray())
            sz *= mDesc.readSize;
    }
    return (uint8_t *)mReadPtr + sz;
}
              
uint8_t *ObjectInfo::nextWritePtr() const
{
    int sz = mDesc.writeSize;
    if (mDesc.wType == String)
    {
        sz = sizeof(_String);
        if (isArray())
            sz *= mDesc.writeSize;
    }
    return (uint8_t *)mWritePtr + sz;
}
