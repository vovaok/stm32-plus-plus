#include "bytearray.h"

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
    mSize = size;
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
    __disable_irq();
    unsigned int desiredSize = mSize + size; // compute desired buffer size
    if (desiredSize <= mAllocSize)
        return;
    while (mAllocSize < desiredSize) // compute nearest allocation size
        mAllocSize = mAllocSize? (mAllocSize << 1): 16; // minimum size = 16 bytes
    //__disable_irq();
    char *temp = new char[mAllocSize]; // allocate new buffer
    //__enable_irq();
    if (mData)
    {
        memcpy(temp, mData, mSize); // copy old data
        //__disable_irq();
        delete mData;               // delete old buffer
        //__enable_irq();
    }
    mData = temp; 
    __enable_irq();
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
    return *this;
}
//---------------------------------------------------------------------------

void ByteArray::resize(int size)
{
    if (size > mAllocSize)
        allocMore(size - mAllocSize);
    mSize = size;
}
//---------------------------------------------------------------------------

void ByteArray::clear()
{
    if (mData)
    {
        __disable_irq();
        delete [] mData;
        __enable_irq();
    }
    mData = 0L;
    mSize = 0;
    mAllocSize = 0;
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
            const char *srcptr = mData + index + count;
            char *destptr = mData + index;
            mSize -= count;
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
