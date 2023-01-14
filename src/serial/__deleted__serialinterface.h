#ifndef _SERIALINTERFACE_H
#define _SERIALINTERFACE_H

#include "core/core.h"

namespace Serial
{

typedef enum
{
    NotOpen     = 0x00,
    Read        = 0x01,
    Write       = 0x02,
    ReadOnly    = Read,
    ReadWrite   = Read | Write
} OpenMode;
  
class SerialInterface
{
protected:
    OpenMode mOpenMode;
  
public:
    SerialInterface() : mOpenMode(NotOpen) {}
  
    virtual bool open(OpenMode mode = ReadWrite) {mOpenMode = mode; return true;}
    virtual void close() {mOpenMode = NotOpen;}
      
    virtual int read(ByteArray &ba) = 0;
    virtual int write(const ByteArray &ba) = 0;
    
    inline bool isOpen() const {return mOpenMode != NotOpen;}
    virtual bool isHalfDuplex() const {return false;}
    
    virtual unsigned char getErrorCode() const {return 0;}
    
    NotifyEvent onReadyRead;
};

}

#endif
