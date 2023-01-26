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
    int write(const char *data, int size);
    
    virtual int bytesAvailable() const = 0; // {return 0;}
    
    bool isOpen() const {return m_openMode;}
    bool isSequential() const {return m_sequential;}
    bool isHalfDuplex() const {return m_halfDuplex;}
    
    NotifyEvent onReadyRead;
    
protected:
    bool m_sequential;
    bool m_halfDuplex;
    
    virtual int readData(char *data, int size) = 0;
    virtual int writeData(const char *data, int size) = 0;
    void setErrorString(const std::string &str);
    
private:
    OpenMode m_openMode;
};

#endif