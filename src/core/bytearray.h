#ifndef _BYTEARRAY_H
#define _BYTEARRAY_H

#include <string.h>
#include "coreexception.h"
#include "stm32f4xx.h"
#include "core_cm4.h"

class ByteArray
{
private:
    char *mData;
    unsigned int mSize;
    unsigned int mAllocSize;
    
    void allocMore(int size);
    
public:
    ByteArray();
    ByteArray(const void *data, unsigned int size);
    ByteArray(const char *str);
    ByteArray(int size, char ch);
    ByteArray(const ByteArray &other);
    ~ByteArray();
    ByteArray &operator=(const ByteArray &other);
    
    ByteArray &append(const void *data, unsigned int size);
    ByteArray &append(const char *str);
    ByteArray &append(const ByteArray &ba);
    ByteArray &append(char byte);
    
    void resize(int size);
    
    void clear();
    
    ByteArray &remove(int index, int count);
    
    inline char *data() {return mData;}
    inline const char *data() const {return mData;}
    inline int size() const {return mSize;}
    inline int length() const {return mSize;}
    inline int count() const {return mSize;}
    
    inline char& operator [](unsigned int idx) {if (idx >= mSize) throw Exception::outOfRange; else return mData[idx];}
    inline const char &operator[](unsigned int idx) const {if (idx >= mSize) throw Exception::outOfRange; else return mData[idx];}
    inline ByteArray &operator +=(const ByteArray &ba) {return append(ba);}
    
    bool operator ==(const ByteArray &ba) const;
    bool operator !=(const ByteArray &ba) const;
};


#endif
