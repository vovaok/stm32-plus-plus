#ifndef _SERIALFRAME_H
#define _SERIALFRAME_H

#include "serialinterface.h"

namespace Serial
{
  
class SerialFrame
{
private:
    SerialInterface *mInterface;
    ByteArray buffer;
    static const char uartSOF = '{';
    static const char uartESC = '\\';
    static const char uartEOF = '}';

    char cs;
    bool esc, cmd_acc;
    unsigned long mFramesSent;
    
    ConstDataEvent mDataReceived;

//    void onByteRead(char byte, SerialInterface *sender);
    void onReadyRead();
    
protected:
    virtual void dataReceived(const ByteArray &ba);

public:
    explicit SerialFrame(SerialInterface *iface);
    unsigned long framesSent() {return mFramesSent;}
    
    void setReceiveEvent(ConstDataEvent event) {mDataReceived = event;}
    ConstDataEvent receiveEvent() const {return mDataReceived;}
    
//    void attach(SerialInterface *iface);

    void sendData(const ByteArray &data);
    
    SerialInterface *getInterface() {return mInterface;}
};

}

#endif 
