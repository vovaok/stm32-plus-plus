#include "bytearray.h"
#include "application.h"

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
//    append(str); // do not copy the string before it is changed
    mData = const_cast<char *>(str);
    mSize = strlen(str);
}

ByteArray::ByteArray(int size, char ch) :
    mData(0L),
    mSize(0),
    mAllocSize(0)
{
    allocMore(size);
    mSize = size;
    char *p = mData;
    while (size--)
        *p++ = ch;
    mData[mSize] = '\0';
}

ByteArray::~ByteArray()
{
    if (mAllocSize)
        clear();
}

ByteArray &ByteArray::operator=(const ByteArray &other)
{
    if (mAllocSize)
        clear();
    else
        mSize = 0;
    append(other);
    return (*this);
}

#if __cplusplus > 199711L
ByteArray::ByteArray(ByteArray &&other)
{
    mData = other.mData;
    mSize = other.mSize;
    mAllocSize = other.mAllocSize;
    other.mData = nullptr;
    other.mSize = 0;
    other.mAllocSize = 0;
}

ByteArray &ByteArray::operator=(ByteArray &&other)
{
    if (mAllocSize)
        clear();
    mData = other.mData;
    mSize = other.mSize;
    mAllocSize = other.mAllocSize;
    other.mData = nullptr;
    other.mSize = 0;
    other.mAllocSize = 0;
    return *this;
}
#endif
//---------------------------------------------------------------------------

char ByteArray::readHex(char *ptr)
{
    char bh = *ptr++ & 0x4F;
    char bl = *ptr++ & 0x4F;
    return ((bh & 0x40? bh - 55: bh) << 4) | (bl & 0x40? bl - 55: bl);
}
//---------------------------------------------------------------------------

void ByteArray::allocMore(int size)
{
    unsigned int desiredSize = mSize + size + 1; // compute desired buffer size // +1 FOR '\0'-terminating string
    if (desiredSize <= mAllocSize)
        return;
    int allocSize = mAllocSize;

    __istate_t interrupt_state = __get_interrupt_state();
    __disable_interrupt();

    if (desiredSize <= 16)
        allocSize = 16;
    if (desiredSize <= 512)
        allocSize = upper_power_of_two(desiredSize);
    else
        allocSize = (desiredSize | 511) + 1;
//    while (allocSize < desiredSize) // compute nearest allocation size
//        allocSize = allocSize? (allocSize << 1): 16; // minimum size = 16 bytes
    char *temp = new char[allocSize]; // allocate new buffer
    if (mData)
    {
        memcpy(temp, mData, mSize); // copy old data
        if (mAllocSize)             // if this ByteArray is the owner of the data
            delete [] mData;        // delete old buffer
    }
    mData = temp;
    mAllocSize = allocSize;

    __set_interrupt_state(interrupt_state);
}
//---------------------------------------------------------------------------

ByteArray &ByteArray::append(const void *data, unsigned int size)
{
    if (size > 0)
    {
        allocMore(size); // relocate buffer by <size> bytes
        const unsigned char *dataptr = reinterpret_cast<const unsigned char*>(data);
        char *mdataptr = mData + mSize;
        mSize += size;
        while (size--)
            *mdataptr++ = *dataptr++;
//        memcpy(mData + mSize, data, size); // copy new data
//        mSize += size;
        mData[mSize] = '\0';
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
    mData[mSize++] = byte;
    mData[mSize] = '\0';
    return *this;
}

ByteArray &ByteArray::prepend(const char *data, unsigned int size)
{
    return insert(0, data, size);
//    if (size <= 0)
//        return *this;
//
//    if (size + mSize >= mAllocSize) // need reallocate, so just create new bytearray
//    {
//        ByteArray ba;
//        ba.allocMore(size + mSize);
//        char *dst = ba.mData;
//        char *src = mData;
//        while (size--)
//            *dst++ = *data++;
//        while (mSize--)
//            *dst++ = *src++;
//        *dst = '\0';
//        *this = std::move(ba);
//    }
//    else
//    {
//        int cnt = mSize;
//        char *src = mData + mSize + 1;
//        mSize += size;
//        char *dst = mData + mSize + 1;
//        while (cnt--)
//            *dst-- = *src--;
//        dst = mData;
//        while (size--)
//            *dst++ = *data++;
//    }
//    return *this;
}

ByteArray &ByteArray::prepend(const char *str)
{
    return prepend(str, strlen(str));
}

ByteArray &ByteArray::prepend(const ByteArray &ba)
{
    return prepend(ba.data(), ba.size());
}

ByteArray &ByteArray::prepend(char byte)
{
    allocMore(1);
    mSize++;
    for (int i=mSize; i>0; --i)
        mData[i] = mData[i-1];
    mData[0] = byte;
    return *this;
}

ByteArray &ByteArray::insert(unsigned int idx, const char *data, unsigned int size)
{
    if (size <= 0)
        return *this;
    if (idx > mSize)
        return *this;

    if (size + mSize >= mAllocSize) // need reallocate, so just create new bytearray
    {
        ByteArray ba;
        ba.resize(size + mSize);
        char *dst = ba.mData;
        char *src = mData;
        mSize -= idx;
        while (idx--)
            *dst++ = *src++;
        while (size--)
            *dst++ = *data++;
        while (mSize--)
            *dst++ = *src++;
        *dst = '\0';
        *this = std::move(ba);
    }
    else
    {
        int cnt = mSize - idx + 1;
        char *src = mData + mSize + 1;
        mSize += size;
        char *dst = mData + mSize + 1;
        while (cnt--)
            *--dst = *--src;
        dst -= size;
        while (size--)
            *dst++ = *data++;
    }
    return *this;
}

ByteArray &ByteArray::insert(unsigned int idx, const char *str)
{
    return insert(idx, str, strlen(str));
}

ByteArray &ByteArray::insert(unsigned int idx, const ByteArray &ba)
{
    return insert(idx, ba.data(), ba.size());
}

ByteArray &ByteArray::insert(unsigned int idx, char byte)
{
    if (idx > mSize)
        return *this;

    allocMore(1);
    mSize++;
    for (int i=mSize; i>idx; --i)
        mData[i] = mData[i-1];
    mData[idx] = byte;
    return *this;
}
//---------------------------------------------------------------------------

void ByteArray::resize(int size)
{
//    int addsize = size - mSize;
    if (size > mSize)
    {
        allocMore(size - mSize);
    }
//    int oldSize = mSize; // used for debug purposes
    mSize = size;
//    size = oldSize;
//    oldSize = addsize;
}
//---------------------------------------------------------------------------

void ByteArray::clear()
{
    __istate_t interrupt_state = __get_interrupt_state();
    __disable_interrupt();

    if (mData && mAllocSize)
        delete [] mData;
    mData = 0L;
    mSize = 0;
    mAllocSize = 0;

    __set_interrupt_state(interrupt_state);
}
//---------------------------------------------------------------------------

ByteArray &ByteArray::remove(int index, int count)
{
    if ((index >= 0) && (index < mSize))
    {
        if (index + count >= mSize)
        {
            mSize = index;
        }
        else
        {
            allocMore(0); // copy data if nececcary before change it
            const char *srcptr = mData + index + count;
            char *destptr = mData + index;
            mSize -= count;
            count = mSize;
            while (count--)
                *destptr++ = *srcptr++;
        }
        mData[mSize] = '\0';
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

void ByteArray::chop(int n)
{
    if (n <= 0)
        return;

    if (n < mSize)
        mSize -= n;
    else
        mSize = 0;
    mData[mSize] = '\0';
}

void ByteArray::truncate(int pos)
{
    if (pos >= 0 && pos < mSize)
        mSize = pos;
    mData[mSize] = '\0';
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

int ByteArray::lastIndexOf(char c, int from) const
{
    if (from < 0)
        from = from + mSize;
    if (from < 0)
        from = 0;
    if (from > 0)
    {
        const char *n = mData + from;
        const char *e = mData;
        while (--n != e)
        if (*n == c)
            return n - mData;
    }
    return -1;
}
//---------------------------------------------------------------------------

ByteArray &ByteArray::replace(const ByteArray &from, const ByteArray &to)
{
    for (int i=0; i<mSize; i+=to.size())
    {
        i = indexOf(from, i);
        if (i < 0)
            break;
        remove(i, from.size());
        insert(i, to);
    }
    return *this;
}

ByteArray &ByteArray::replace(char from, char to)
{
    allocMore(0); // copy data if nececcary before change it
    for (int i=0; i<mSize; i++)
    {
        i = indexOf(from, i);
        if (i < 0)
            break;
        mData[i] = to;
    }
    return *this;
}
//---------------------------------------------------------------------------

int ByteArray::toInt() const
{
    if (!mSize)
        return 0; // alarm
    int value;
    sscanf(mData, "%d", &value);
    return value;
}

float ByteArray::toFloat() const
{
    if (!mSize)
        return 0; // alarm
    float value;
    sscanf(mData, "%f", &value);
    return value;
}

std::string ByteArray::toStdString() const
{
    return std::string(mData, mSize);
}
//---------------------------------------------------------------------------

ByteArray ByteArray::fromStdString(const std::string &str)
{
    return ByteArray(str.c_str(), str.size());
}

ByteArray ByteArray::fromRawData(const char *data, int size)
{
    ByteArray ba;
    ba.mData = const_cast<char *>(data);
    ba.mSize = size;
    return ba;
}

ByteArray ByteArray::fromPercentEncoding(const ByteArray &ba)
{
    ByteArray out;
    char *ptr = ba.mData;
    for (int i=0; i<ba.mSize; i++)
    {
        if (*ptr == '%')
        {
            out.append(readHex(ptr++));
            i += 2;
            ptr += 2;
        }
        else
        {
            out.append(*ptr++);
        }
    }
    return out;
}

ByteArray ByteArray::number(int n)
{
    ByteArray ba;
    ba.resize(16);
    sprintf(ba.data(), "%d", n);
    ba.resize(strlen(ba.data()));
    return ba;
}

ByteArray ByteArray::number(float n, char f, int prec)
{
    ByteArray ba;
    ba.resize(16);
    char fmt[8];
    if (prec >= 0)
    {
        sprintf(fmt, "%%.%d%c", prec, f);
        sprintf(ba.data(), fmt, n);
    }
    else
    {
        sprintf(fmt, "%%%c", f);
        sprintf(ba.data(), fmt, n);
    }
    ba.resize(strlen(ba.data()));
    return ba;
}

ByteArray ByteArray::number(double n, char f, int prec)
{
    ByteArray ba;
    ba.resize(16);
    char fmt[8];
    if (prec >= 0)
    {
        sprintf(fmt, "%%.%d%c", prec, f);
        sprintf(ba.data(), fmt, n);
    }
    else
    {
        sprintf(fmt, "%%%c", f);
        sprintf(ba.data(), fmt, n);
    }
    ba.resize(strlen(ba.data()));
    return ba;
}
//---------------------------------------------------------------------------

ByteArray operator +(const ByteArray &ba1, const ByteArray ba2)
{
    return ByteArray(ba1).append(ba2);
}

bool operator<(const ByteArray &ba1, const ByteArray &ba2)
{
    int n = ba1.mSize > ba2.mSize? ba1.mSize: ba2.mSize;
    return strncmp(ba1.mData, ba2.mData, n) < 0;
}