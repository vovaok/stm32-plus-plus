#ifndef _DEVICE_H
#define _DEVICE_H

#include <string>
#include "coretypes.h"

class Device
{
public:
    typedef enum
    {
        NotOpen     = 0x0000,
        ReadOnly    = 0x0001,
        WriteOnly   = 0x0002,
        
        ReadWrite = ReadOnly | WriteOnly
    } OpenMode;

public:
    Device();
    
    virtual bool open(OpenMode mode=ReadWrite);
    virtual void close();
    
    int read(char *data, int maxsize);
    ByteArray read(int maxsize);
    ByteArray readAll();
    int write(const char *data, int size);
    int write(const char *data); // null-terminated string
    int write(const ByteArray &data);
    
    virtual uint32_t pos() const {return 0;}
    virtual bool seek(uint32_t pos) {return false;}
    
    virtual bool atEnd() const {return !bytesAvailable();}
    virtual int bytesAvailable() const = 0; // {return 0;}
    
    bool isOpen() const {return m_openMode;}
    bool isSequential() const {return m_sequential;}
    bool isHalfDuplex() const {return m_halfDuplex;}
    bool isReadable() const {return m_openMode & ReadOnly;}
    bool isWritable() const {return m_openMode & WriteOnly;}
    
    virtual bool canReadLine() const {return false;}
    int readLine(char *data, int maxsize);
    ByteArray readLine(int maxsize=0);
    
    NotifyEvent onReadyRead;
    NotifyEvent onBytesWritten;
    
    const ByteArray &errorString() const {return m_errorString;}
    
protected:
    bool m_sequential;
    bool m_halfDuplex;
    
    virtual int readData(char *data, int size) = 0;
    virtual int writeData(const char *data, int size) = 0;
    void setErrorString(const ByteArray &str) {m_errorString = str;}
    virtual int readLineData(char *data, int size);
    
private:
    OpenMode m_openMode;
    ByteArray m_errorString;
};

#endif