#ifndef _RADIOINTERFACE_H
#define _RADIOINTERFACE_H

#include "cc1200.h"
#include "serial/serialinterface.h"

using namespace Serial;

class RadioInterface : public SerialInterface
{
private:
    CC1200 *cc1200;
    ByteArray mRxBuffer;
    
    void task();
  
public:
    RadioInterface(CC1200 *device);
    
    virtual bool open(OpenMode mode = ReadWrite);
    virtual void close();
      
    virtual int read(ByteArray &ba);
    virtual int write(const ByteArray &ba);
    
    virtual bool isHalfDuplex() const {return true;}
};

#endif