#ifndef _BYTEARRAY_H
#define _BYTEARRAY_H

#include <string>
#include <string.h>
#include <stdio.h>
#include "coreexception.h"

class ByteArray
{
private:
    char *mData;
    unsigned int mSize;
    unsigned int mAllocSize;

    void allocMore(int size);
    static char readHex(char *ptr);
    static void writeHex(char *&ptr, char value);

public:
    ByteArray();
    ByteArray(const void *data, unsigned int size);
    ByteArray(const char *str);
    ByteArray(int size, char ch);
    ByteArray(std::initializer_list<char> il);
    ~ByteArray();
    ByteArray(const ByteArray &other);
    ByteArray &operator=(const ByteArray &other);
#if __cplusplus > 199711L
    ByteArray(ByteArray &&other);
    ByteArray &operator=(ByteArray &&other);
#endif

    ByteArray &append(const void *data, unsigned int size);
    ByteArray &append(const char *str);
    ByteArray &append(const ByteArray &ba);
    ByteArray &append(char byte);

    ByteArray &prepend(const char *data, unsigned int size);
    ByteArray &prepend(const char *str);
    ByteArray &prepend(const ByteArray &ba);
    ByteArray &prepend(char byte);

    ByteArray &insert(unsigned int idx, const char *data, unsigned int size);
    ByteArray &insert(unsigned int idx, const char *str);
    ByteArray &insert(unsigned int idx, const ByteArray &ba);
    ByteArray &insert(unsigned int idx, char byte);

    void resize(int size);

    void clear();

    ByteArray &remove(int index, int count);

    inline char *data() {return mData;}
    inline const char *data() const {return mData;}
    inline int size() const {return mSize;}
    inline int length() const {return mSize;}
    inline int count() const {return mSize;}
    inline bool isEmpty() const {return !mSize;}

    inline int capacity() const {return mAllocSize;}

#if defined(__CPP_EXCEPTIONS__) | defined(__CPP_Exceptions) | defined(__cpp_exceptions)
    inline char& operator [](unsigned int idx) {if (idx >= mSize) throw Exception::OutOfRange; else return mData[idx];}
    inline const char &operator[](unsigned int idx) const {if (idx >= mSize) throw Exception::OutOfRange; else return mData[idx];}
    inline char at(unsigned int idx) const {if (idx >= mSize) throw Exception::OutOfRange; else return mData[idx];}
#else
    inline char& operator [](unsigned int idx) {return mData[idx];}
    inline const char &operator[](unsigned int idx) const {return mData[idx];}
    char at(unsigned int idx) const {return mData[idx];}
#endif

    inline ByteArray &operator +=(const ByteArray &ba) {return append(ba);}
    friend ByteArray operator +(const ByteArray &ba1, const ByteArray ba2);
    friend bool operator<(const ByteArray &a1, const ByteArray &a2);

    bool operator ==(const ByteArray &ba) const;
    bool operator !=(const ByteArray &ba) const;

    ByteArray left(int len) const;
    ByteArray right(int len) const;
    ByteArray mid(int index, int len = -1) const;
    void chop(int n);
    void truncate(int pos);

    bool startsWith(const ByteArray &ba) const;
    bool startsWith(char c) const;
    bool startsWith(const char *str) const;

    bool endsWith(const ByteArray &ba) const;
    bool endsWith(char c) const;
    bool endsWith(const char *str) const;

    inline bool contains(const ByteArray &a) const{ return bool(indexOf(a) != -1); }
    inline bool contains(char c) const { return bool(indexOf(c) != -1); }
    inline bool contains(const char *str) const { return bool(indexOf(str) != -1); }

    int indexOf(char c, int from = 0) const;
    int indexOf(const char *str, int from = 0) const;
    int indexOf(const ByteArray &ba, int from = 0) const;

    int lastIndexOf(char c, int from = -1) const;

    ByteArray &replace(const ByteArray &from, const ByteArray &to);
    ByteArray &replace(char from, char to);

    int toInt() const;
    int toInt(int base) const;
    float toFloat() const;
    std::string toStdString() const;
    ByteArray toHex();
    ByteArray toHex(char separator);

    static ByteArray fromHex(const ByteArray &ba);
    static ByteArray fromStdString(const std::string &str);
    static ByteArray fromRawData(const char *data, int size);
    static ByteArray fromPercentEncoding(const ByteArray &ba);
    static ByteArray number(int n);
    static ByteArray number(float n, char f='g', int prec=-1);
    static ByteArray number(double n, char f='g', int prec=-1);
};

ByteArray operator +(const ByteArray &ba1, const ByteArray ba2);
bool operator<(const ByteArray &ba1, const ByteArray &ba2);

#endif