#ifndef _USBVCP_H
#define _USBVCP_H

#include "usb/usb.h"
#include "usb/usbcdc/usbcdc.h"
#include "core/device.h"
#include "usb/usbifcol.h"
#include "cpuid.h"
#include "core/application.h"

using namespace Usb;

#pragma pack(4)
class UsbVcp : public Device
{
private:
    UsbDevice *mDev;
    UsbConfiguration *mCfg;
    UsbVcpCtrlInterface *mIfCtrl;
    UsbVcpDataInterface *mIfData;
    
    ByteArray mBuffer;
    int mBufSize;
    int mBufHead, mBufTail;
    
    void initDevice();
    void onReceive(const ByteArray &ba);
    
public:
    // vid/pid = 0x0483/0x5740 for STM VCP
    UsbVcp(UsbDevice::UsbCore usb, unsigned short vid=0x0483, unsigned short pid=0x5740);    
    UsbVcp(UsbDevice::UsbCore usb, const char *serial);
    UsbVcp(UsbConfiguration *usbconf);
    ~UsbVcp();
    
//    int write(const ByteArray &ba);
//    int read(ByteArray &ba);
    
    int readData(char *data, int size);
    int writeData(const char *data, int size);
    
    UsbConfiguration *usbCfgNode() {return mCfg;}
    //void setReadEvent(const ConstDataEvent &e) {mIfData->setReceiveEvent(e);}
    
protected:
    virtual int bytesAvailable() const;
};

#endif