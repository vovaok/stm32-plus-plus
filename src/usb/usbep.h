#ifndef _USBEP_H
#define _USBEP_H

#include "usbdesc.h"
#include "usbnode.h"
#include "core/core.h"

namespace Usb
{
  
//class UsbCsEndpoint
//{
//private:
//    ClassSpecificDescriptor *mDescriptor;
//    
//public:
//    UsbCsEndpoint(ClassSpecificDescriptor *descriptor) {mDescriptor = descriptor;}
//};
//---------------------------------------------------------------------------

class UsbEndpoint : public UsbNode
{
private:
    EndpointDescriptor *mDescriptor;
    ByteArray mRxBuffer;
    ByteArray mTxBuffer;
    ConstDataEvent mDataOutEvent;
    //void getRxBuffer(ByteArray &data);
    
protected:
    //UsbCsEndpoint *csEndpoint;
    void init();
    void deInit();
    void dataIn();
    void dataOut();
    
    friend class UsbDevice;
    
public:
    UsbEndpoint(EndpointDirection epDir, TransferType transferType, unsigned short maxPacketSize, unsigned char pollInterval=1);
    
    void setEndpointNumber(unsigned char num) {mDescriptor->setEndpointAddress((mDescriptor->endpointAddress() & 0x80) | (num & 0x7F));}
    unsigned char number() const {return mDescriptor->endpointAddress();}
    
    void setDataOutEvent(const ConstDataEvent &e) {mDataOutEvent = e;}
    void sendData(const ByteArray &data);
    
    bool isIn() const {return mDescriptor->endpointAddress() & 0x80;}
};
//
//class UsbAltSetting : public UsbNode
//{
//private:
//  
//public:
//    
//};

};

#endif