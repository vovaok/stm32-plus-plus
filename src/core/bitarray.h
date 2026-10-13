#pragma once

#include "bytearray.h"

#define ASSERT(x)  if (!(x)) THROW(Exception::OutOfRange)

class QBitRef;
class QBitArray
{
    typedef unsigned char uchar;
    typedef unsigned int uint;
    
    ByteArray d;

public:
    inline QBitArray() noexcept {}
    explicit QBitArray(int size, bool val = false);
    QBitArray(const QBitArray &other) : d(other.d) {}
    inline QBitArray &operator=(const QBitArray &other) { d = other.d; return *this; }
    inline QBitArray(QBitArray &&other) noexcept : d(std::move(other.d)) {}
    inline QBitArray &operator=(QBitArray &&other) noexcept
    { std::swap(d, other.d); return *this; }

    inline void swap(QBitArray &other) noexcept { std::swap(d, other.d); }

    inline int size() const { return (d.size() << 3) - *d.data(); }
    inline int count() const { return (d.size() << 3) - *d.data(); }
    int count(bool on) const;

    inline bool isEmpty() const { return d.isEmpty(); }
//    inline bool isNull() const { return d.isNull(); }

    void resize(int size);

//    inline void detach() { d.detach(); }
//    inline bool isDetached() const { return d.isDetached(); }
    inline void clear() { d.clear(); }

    bool testBit(int i) const;
    void setBit(int i);
    void setBit(int i, bool val);
    void clearBit(int i);
    bool toggleBit(int i);

    bool at(int i) const;
    QBitRef operator[](int i);
    bool operator[](int i) const;
    QBitRef operator[](uint i);
    bool operator[](uint i) const;

    QBitArray& operator&=(const QBitArray &);
    QBitArray& operator|=(const QBitArray &);
    QBitArray& operator^=(const QBitArray &);
    QBitArray  operator~() const;

    inline bool operator==(const QBitArray& other) const { return d == other.d; }
    inline bool operator!=(const QBitArray& other) const { return d != other.d; }

    inline bool fill(bool val, int size = -1);
//    void fill(bool val, int first, int last);

    inline void truncate(int pos) { if (pos < size()) resize(pos); }

    const char *bits() const { return isEmpty() ? nullptr : d.data() + 1; }
//    static QBitArray fromBits(const char *data, int len);

public:
//    typedef ByteArray::DataPtr DataPtr;
//    inline DataPtr &data_ptr() { return d.data_ptr(); }
};

inline bool QBitArray::fill(bool aval, int asize)
{ *this = QBitArray((asize < 0 ? this->size() : asize), aval); return true; }

QBitArray operator&(const QBitArray &, const QBitArray &);
QBitArray operator|(const QBitArray &, const QBitArray &);
QBitArray operator^(const QBitArray &, const QBitArray &);

inline bool QBitArray::testBit(int i) const
{ ASSERT(uint(i) < uint(size()));
 return (*(reinterpret_cast<const uchar*>(d.data())+1+(i>>3)) & (1 << (i & 7))) != 0; }

inline void QBitArray::setBit(int i)
{ ASSERT(uint(i) < uint(size()));
 *(reinterpret_cast<uchar*>(d.data())+1+(i>>3)) |= uchar(1 << (i & 7)); }

inline void QBitArray::clearBit(int i)
{ ASSERT(uint(i) < uint(size()));
 *(reinterpret_cast<uchar*>(d.data())+1+(i>>3)) &= ~uchar(1 << (i & 7)); }

inline void QBitArray::setBit(int i, bool val)
{ if (val) setBit(i); else clearBit(i); }

inline bool QBitArray::toggleBit(int i)
{ ASSERT(uint(i) < uint(size()));
 uchar b = uchar(1<<(i&7)); uchar* p = reinterpret_cast<uchar*>(d.data())+1+(i>>3);
 uchar c = uchar(*p&b); *p^=b; return c!=0; }

inline bool QBitArray::operator[](int i) const { return testBit(i); }
inline bool QBitArray::operator[](uint i) const { return testBit(i); }
inline bool QBitArray::at(int i) const { return testBit(i); }

class QBitRef
{
private:
    QBitArray& a;
    int i;
    inline QBitRef(QBitArray& array, int idx) : a(array), i(idx) {}
    friend class QBitArray;
public:
    QBitRef(const QBitRef &) = default;
    inline operator bool() const { return a.testBit(i); }
    inline bool operator!() const { return !a.testBit(i); }
    QBitRef& operator=(const QBitRef& val) { a.setBit(i, val); return *this; }
    QBitRef& operator=(bool val) { a.setBit(i, val); return *this; }
};

inline QBitRef QBitArray::operator[](int i)
{ ASSERT(i >= 0); return QBitRef(*this, i); }
inline QBitRef QBitArray::operator[](uint i)
{ return QBitRef(*this, i); }


//class BitArray
//{
//public:
//    inline BitArray() noexcept {}
//    explicit BitArray(int size, bool value=false);
//    BitArray(const BitArray &other) : m_data(other.m_data) {}
//    inline BitArray &operator=(const BitArray &other)
//    { m_data = other.m_data; return *this; }
//    inline BitArray(BitArray &&other) noexcept : m_data(std::move(m_data)) {}
//    inline BitArray &operator=(BitArray &&other) noexcept
//    { std::swap(m_data, other.m_data); return *this; }
//    
//    inline int size() const {return m_size;}
//    void resize(int size);
//    
//private:
//    ByteArray m_data;
//    int m_size; // bit count
//};