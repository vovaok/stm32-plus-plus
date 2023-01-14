#ifndef _SERIALFRAME_H
#define _SERIALFRAME_H

#include "core/device.h"
#include "core/bytearray.h"
#include "core/coretypes.h"
#include "application.h"

namespace Serial
{
  
class SerialFrame
{
private:
    Device *m_device;
    ByteArray m_buffer;
    static const char uartSOF = '{';
    static const char uartESC = '\\';
    static const char uartEOF = '}';

    char cs;
    bool esc, cmd_acc;
    unsigned long mFramesSent;

    void task();
    
protected:
    virtual void dataReceived(const ByteArray &ba);

public:
    explicit SerialFrame(Device *iface);
    unsigned long framesSent() {return mFramesSent;}
    
    ConstDataEvent onDataReceived;
    
//    void attach(SerialInterface *iface);

    void sendData(const ByteArray &data);
    
    Device *getInterface() {return m_device;}
};

}

#endif 
