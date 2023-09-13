#ifndef _USBHIDONBINTERFACE_H
#define _USBHIDONBINTERFACE_H

#include <deque>
#include "objnetInterface.h"
#include "usb/usbHid/usbhid.h"
#include "core/timer.h"

using namespace Usb;

namespace Objnet
{
  
#pragma pack(push,1)
typedef struct
{
    unsigned long id;
    unsigned char size;
    unsigned char data[8];
} UsbHidOnbMessage;
#pragma pack(pop)
  
class UsbHidOnbInterface : public ObjnetInterface
{
private:
    UsbHidInterface *mHidIf;
    UsbEndpoint *mHidEpIn;
    UsbEndpoint *mHidEpOut;
    Timer *mReportTimer;
    
    ByteArray __createReportDescriptor(unsigned char id, unsigned char size);
    ByteArray createInputReportDescriptor(unsigned char id, unsigned char size);
    ByteArray createOutputReportDescriptor(unsigned char id, unsigned char size);
    ByteArray createFeatureReportDescriptor(unsigned char id, unsigned char size);
    void fillReportDescriptor(ByteArray &ba);
    
    std::deque<UsbHidOnbMessage> mTxQueue;
    std::deque<UsbHidOnbMessage> mRxQueue;
    const static int mTxQueueSize = 128;
    const static int mRxQueueSize = 64;
    unsigned char mSeqNo;
    
    bool readRx(UsbHidOnbMessage &msg);
    bool writeRx(UsbHidOnbMessage &msg);
    bool readTx(UsbHidOnbMessage &msg);
    bool writeTx(UsbHidOnbMessage &msg);
    
    void onTimer();
    void onSetReport(int reportId, ByteArray &ba);
    void onGetReport(int reportId, ByteArray &ba);

public:
    explicit UsbHidOnbInterface(UsbInterface *usbif);
//    virtual ~UsbHidOnbInterface();

    bool write(const CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();
    
    int availableWriteCount();

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    bool readyToTransmit() const {return !mTxQueue.empty();}
};

}

#endif // USBHIDONBINTERFACE_H
