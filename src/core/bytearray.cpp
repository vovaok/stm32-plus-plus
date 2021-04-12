#include "bytearray.h"

//#define ASSERT_SIZE()   if (mSize > mAllocSize) while (1)

ByteArray::ByteArray() :
    mData(0L),
    mSize(0),
    mAllocSize(0)
{
}

ByteArray::ByteArray(const ByteArray &other) :
    mData(0L),
    mSize(0),
    mAllocSize(0)
{
    append(other);
}

ByteArray::ByteArray(const void *data, unsigned int size) :
    mData(0L),
    mSize(0),
    mAllocSize(0)
{
    append(data, size);
}

ByteArray::ByteArray(const char *str) :
    mData(0L),
    mSize(0),
    mAllocSize(0)
{
    append(str);
}

ByteArray::ByteArray(int size, char ch) :
    mData(0L),
    mSize(0),
    mAllocSize(0)
{
    allocMore(size);
//    ASSERT_SIZE();
    mSize = size;
//    ASSERT_SIZE();
    char *p = mData;
    while (size--)
        *p++ = ch;
}

ByteArray::~ByteArray()
{
    clear();
}

ByteArray &ByteArray::operator=(const ByteArray &other)
{
    clear();
    append(other);
    return (*this);
}
//---------------------------------------------------------------------------

void ByteArray::allocMore(int size)
{ 
    unsigned int desiredSize = mSize + size; // compute desired buffer size
//    ASSERT_SIZE();
    if (desiredSize <= mAllocSize)
        return;
    unsigned long _primask = __get_PRIMASK();
    __disable_irq();
    while (mAllocSize < desiredSize) // compute nearest allocation size
        mAllocSize = mAllocSize? (mAllocSize << 1): 16; // minimum size = 16 bytes
//    ASSERT_SIZE();
    char *temp = new char[mAllocSize]; // allocate new buffer
    if (mData)
    {
        memcpy(temp, mData, mSize); // copy old data
        delete [] mData;               // delete old buffer
    }
    mData = temp; 
    if (!_primask)
        __enable_irq();
}
//---------------------------------------------------------------------------

ByteArray &ByteArray::append(const void *data, unsigned int size)
{
    if (size > 0)
    {
        allocMore(size); // relocate buffer by <size> bytes
//        ASSERT_SIZE();
        const unsigned char *dataptr = reinterpret_cast<const unsigned char*>(data);
        char *mdataptr = mData + mSize;
        mSize += size;
//        ASSERT_SIZE();
        while (size--)
            *mdataptr++ = *dataptr++;
//        memcpy(mData + mSize, data, size); // copy new data    
//        mSize += size;
    }
    return *this;
}

ByteArray &ByteArray::append(const char *str)
{
    return append(str, strlen(str));
}

ByteArray &ByteArray::append(const ByteArray &ba)
{
    return append(ba.data(), ba.size());
}

ByteArray &ByteArray::append(char byte)
{
    allocMore(1);
//    ASSERT_SIZE();
    mData[mSize++] = byte;
//    ASSERT_SIZE();
    return *this;
}
//---------------------------------------------------------------------------

void ByteArray::resize(int size)
{
//    unsigned long _primask = __get_PRIMASK();
//    __disable_irq();
    int addsize = size - mSize;
    if (size > mSize)
    {
        allocMore(size - mSize);
//        ASSERT_SIZE();
    }
    int oldSize = mSize;
    mSize = size;
//    ASSERT_SIZE();
    size = oldSize;
    oldSize = addsize;
//    if (!_primask)
//        __enable_irq();
}
//---------------------------------------------------------------------------

void ByteArray::clear()
{
    unsigned long _primask = __get_PRIMASK();
    __disable_irq();
    if (mData)
    {
        delete [] mData;
        mData = 0L;
    }
    mSize = 0;
//    ASSERT_SIZE();
    mAllocSize = 0;
//    ASSERT_SIZE();
    if (!_primask)
        __enable_irq();
}
//---------------------------------------------------------------------------

ByteArray &ByteArray::remove(int index, int count)
{
    if ((index >= 0) && (index < mSize))
    {
        if (index + count >= mSize)
        {
            mSize = index;
//            ASSERT_SIZE();
        }
        else
        {
            const char *srcptr = mData + index + count;
            char *destptr = mData + index;
            mSize -= count;
//            ASSERT_SIZE();
            count = mSize;
            while (count--)
                *destptr++ = *srcptr++;
        }
    }
    return *this;
}
//---------------------------------------------------------------------------

bool ByteArray::operator ==(const ByteArray &ba) const
{
    if (ba.mSize != mSize)
        return false;
    for (int i=0; i<mSize; i++)
        if (ba.mData[i] != mData[i])
            return false;
    return true;
}

bool ByteArray::operator !=(const ByteArray &ba) const
{
    if (ba.mSize != mSize)
        return true;
    for (int i=0; i<mSize; i++)
        if (ba.mData[i] != mData[i])
            return true;
    return false;
}
//---------------------------------------------------------------------------

bool ByteArray::startsWith(const ByteArray &ba) const
{
    if (ba.mSize == 0)
        return true;
    if (mSize < ba.mSize)
        return false;
    return memcmp(mData, ba.mData, ba.mSize) == 0;
}

bool ByteArray::startsWith(const char *str) const
{
    if (!str || !*str)
        return true;
    int len = strlen(str);
    if (mSize < len)
        return false;
    return strncmp(mData, str, len) == 0;
}

bool ByteArray::startsWith(char ch) const
{
    if (mSize == 0)
        return false;
    return mData[0] == ch;
}

bool ByteArray::endsWith(const ByteArray &ba) const
{
    if (ba.mSize == 0)
        return true;
    if (mSize < ba.mSize)
        return false;
    return memcmp(mData + mSize - ba.mSize, ba.mData, ba.mSize) == 0;
}

bool ByteArray::endsWith(const char *str) const
{
    if (!str || !*str)
        return true;
    int len = strlen(str);
    if (mSize < len)
        return false;
    return strncmp(mData + mSize - len, str, len) == 0;
}

bool ByteArray::endsWith(char ch) const
{
    if (mSize == 0)
        return false;
    return mData[mSize - 1] == ch;
}

ByteArray ByteArray::left(int len) const
{
    if (len >= mSize)
        return *this;
    if (len < 0)
        len = 0;
    return ByteArray(mData, len);
}

ByteArray ByteArray::right(int len) const
{
    if (len >= mSize)
        return *this;
    if (len < 0)
        len = 0;
    return ByteArray(mData + mSize - len, len);
}

ByteArray ByteArray::mid(int pos, int len) const
{
    if (pos >= mSize)
        return ByteArray();
    if (len < 0)
        len = mSize - pos;
    if (pos < 0) {
        len += pos;
        pos = 0;
    }
    if (len + pos > mSize)
        len = mSize - pos;
    if (pos == 0 && len == mSize)
        return *this;
    return ByteArray(mData + pos, len);
}
//---------------------------------------------------------------------------

int findByteArray(const char *b, int blen, int from, const char *s, int slen)
{
    for (int i=from; i<=blen-slen; i++)
    {
        int j;
        for (j=0; j<slen && (b[i+j]==s[j]); j++);
        if (j==slen)
            return i;
    }
    return -1;
}

int ByteArray::indexOf(const ByteArray &ba, int from) const
{
    const int ol = ba.mSize;
    if (ol == 0)
        return from;
    if (ol == 1)
        return indexOf(*ba.mData, from);

    const int l = mSize;
    if (from > l || ol + from > l)
        return -1;

    return findByteArray(mData, mSize, from, ba.mData, ol);
}

int ByteArray::indexOf(const char *str, int from) const
{
    const int ol = strlen(str);
    if (ol == 1)
        return indexOf(*str, from);
    
    const int l = mSize;
    if (from > l || ol + from > l)
        return -1;
    if (ol == 0)
        return from;

    return findByteArray(mData, mSize, from, str, ol);
}

int ByteArray::indexOf(char c, int from) const
{
    if (from < 0)
        from = from + mSize;
    if (from < 0)
        from = 0;
    if (from < mSize)
    {
        const char *n = mData + from - 1;
        const char *e = mData + mSize;
        while (++n != e)
        if (*n == c)
            return n - mData;
    }
    return -1;
}
//---------------------------------------------------------------------------
