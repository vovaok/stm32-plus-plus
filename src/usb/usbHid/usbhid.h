#ifndef _USBHID_H
#define _USBHID_H

#include "../usbdesc.h"
#include "../usbep.h"
#include "../usbif.h"
#include "../usbcfg.h"
#include "../usbdev.h" // poka tak
#include "hiditem.h"

#define HID_DESCRIPTOR_TYPE     0x21
#define HID_REPORT_DESC         0x22

#define HID_REQ_SET_PROTOCOL    0x0B
#define HID_REQ_GET_PROTOCOL    0x03
#define HID_REQ_SET_IDLE        0x0A
#define HID_REQ_GET_IDLE        0x02
#define HID_REQ_SET_REPORT      0x09
#define HID_REQ_GET_REPORT      0x01

namespace Usb
{
  
typedef Closure<void(int, ByteArray &)> NumberedDataEvent;
typedef Closure<void(int, ByteArray &)> NumberedDataEvent;
  
typedef enum
{
    HidSubclassNoBoot   = 0x00,
    HidSubclassBoot     = 0x01
} HidSubclass;

typedef enum
{
    HidProtocolNone     = 0x00,
    HidProtocolKeyboard = 0x01,
    HidProtocolMouse    = 0x02
} HidProtocol;

typedef enum
{
    HidReportTypeInput  = 0x01,
    HidReportTypeOutput = 0x02,
    HidReportTypeFeature= 0x03,
} HidReportType;

class HidInterfaceDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 9
        unsigned char bDescriptorType;  // descriptor type is HID interface descriptor
        unsigned short bcdHID;
        unsigned char bCountryCode;
        unsigned char bNumDescriptors;
        unsigned char bHidDescriptorType; // poka 4to BOT TAK BOT. (B  C M bl C JI E   1 descriptor)
        unsigned short wItemLength;
    } Fields;
#pragma pack(pop) 
    
    void setReportSize(int reportSize) {reinterpret_cast<Fields*>(data())->wItemLength = reportSize;}
    
//public:
    HidInterfaceDescriptor() :
      Descriptor(9)
//      bcdHID(0x0111),
//      bCountryCode(0),
//      bNumDescriptors(1),
//      bHidDescriptorType(0x22),
//      wItemLength(0)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 9;
        m->bDescriptorType = HID_DESCRIPTOR_TYPE;
        m->bcdHID = 0x0111;
        m->bNumDescriptors = 1;
        m->bHidDescriptorType = 0x22;
    }
    ~HidInterfaceDescriptor()
    { 
    }
    
    friend class UsbHidInterface;
};

class UsbHidInterface : public UsbNode //UsbCsInterface
{
private:
    HidInterfaceDescriptor *mDescriptor;
    ByteArray mReport;
    UsbEndpoint *mInEp, *mOutEp;
    UsbSetupReq mReqBuffer;
    ByteArray mCtlBuffer;
    bool mMultipleReportId;
    
    void setReport(const ByteArray &report);
    
    unsigned char mProtocol;
    unsigned char mIdleState;
    unsigned char mAltSet;
  
protected:
    void addChild(UsbNode *child);
    void setup(const UsbSetupReq &req);
    void ep0RxReady();
    
public:
    UsbHidInterface(const ByteArray &reportDescriptor);
    
    void setUseMultipleReportId(bool enable) {mMultipleReportId = enable;}
    bool isUseMultipleReportId() const {return mMultipleReportId;}
    
    ConstDataEvent onReportReceiveEvent; //(const ByteArray &ba);
    NumberedDataEvent onSetReportEvent; //(int reportId), ByteArray &ba);
    NumberedDataEvent onGetReportEvent; //(int reportId), ByteArray &ba);
    
    void sendReport(const ByteArray &report);
    void onReportReceive(const ByteArray &report);
};

}

#endif