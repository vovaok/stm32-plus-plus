#ifndef _BYTEARRAY_H
#define _BYTEARRAY_H

#include <string.h>
#include "coreexception.h"
#if defined(STM32F37X)
    #include "stm32f37x.h"
#else
    #include "stm32f4xx.h"
#endif
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
    
#ifndef NO_EXCEPTIONS
    inline char& operator [](unsigned int idx) {if (idx >= mSize) throw Exception::outOfRange; else return mData[idx];}
    inline const char &operator[](unsigned int idx) const {if (idx >= mSize) throw Exception::outOfRange; else return mData[idx];}
#else
    inline char& operator [](unsigned int idx) {return mData[idx];}
    inline const char &operator[](unsigned int idx) const {return mData[idx];}
#endif

    inline ByteArray &operator +=(const ByteArray &ba) {return append(ba);}
    
    bool operator ==(const ByteArray &ba) const;
    bool operator !=(const ByteArray &ba) const;
    
    ByteArray left(int len) const;
    ByteArray right(int len) const;
    ByteArray mid(int index, int len = -1) const;

    bool startsWith(const ByteArray &ba) const;
    bool startsWith(char c) const;
    bool startsWith(const char *str) const;

    bool endsWith(const ByteArray &ba) const;
    bool endsWith(char c) const;
    bool endsWith(const char *str) const;
    
    int indexOf(char c, int from = 0) const;
    int indexOf(const char *str, int from = 0) const;
    int indexOf(const ByteArray &ba, int from = 0) const;
};


#endif
