#ifndef _USBDEVDRV_H
#define _USBDEVDRV_H

#include "core/core.h"
#include "otgDriver/usb_core.h"
#include "otgDriver/usb_bsp.h"

namespace Usb
{
  
typedef enum
{
    UsbOtgDefault       = 1,
    UsbOtgAddressed     = 2,
    UsbOtgConfigured    = 3,
    UsbOtgSuspended     = 4
} UsbOtgStatus;
  
typedef Closure<void(unsigned char)> EndpointEvent;

class UsbDeviceDriver
{
private:
    typedef struct
    {
        uint8_t  bLength;
        uint8_t  bDescriptorType;
        uint8_t  bEndpointAddress;
        uint8_t  bmAttributes;
        uint16_t wMaxPacketSize;
        uint8_t  bInterval;
    }
    EP_DESCRIPTOR, *PEP_DESCRIPTOR;
  
private:
    USB_OTG_CORE_HANDLE *pdev;
    ByteArray mTxBuffer;
  
    uint32_t readDevInEp(uint8_t epnum);

    // interrupt Handlers
    uint32_t handleInEp();
    uint32_t handleOutEp();
    uint32_t handleSof();

    uint32_t handleRxStatusQueueLevel();
    uint32_t writeEmptyTxFifo(uint32_t epnum);

    uint32_t handleUsbReset();
    uint32_t handleEnumDone();
    uint32_t handleResume();
    uint32_t handleUsbSuspend();

    uint32_t handleIsoInIncomplete();
    uint32_t handleIsoOutIncomplete();
    
#ifdef VBUS_SENSING_ENABLED
    uint32_t sessionRequest();
    uint32_t otgIsr();
#endif
    
public:
    UsbDeviceDriver(USB_OTG_CORE_HANDLE *handle);
    virtual ~UsbDeviceDriver();
    
    void isrHandler();
    
    void init(USB_OTG_CORE_ID_TypeDef coreId);
    void devConnect();
    void devDisconnect();
    void epSetAddress(uint8_t address);
    uint32_t epOpen(uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type);
    uint32_t epClose(uint8_t ep_addr);
    uint32_t epPrepareRx(uint8_t ep_addr, void *pbuf, uint16_t buf_len);
    uint32_t epTx(uint8_t ep_addr, void *pbuf, uint32_t buf_len);
    uint32_t epStall(uint8_t epnum);
    uint32_t epClrStall(uint8_t epnum);
    uint32_t epFlush(uint8_t epnum);
    uint32_t getEpStatus(uint8_t epnum);
//    uint32_t getEpXferCount(uint8_t epnum);
    void setEpStatus(uint8_t epnum, uint32_t Status);
    
    EndpointEvent dataOutStageEvent; // void dataOutStage(unsigned char epnum);
    EndpointEvent dataInStageEvent; // void dataInStage(unsigned char epnum);
    NotifyEvent setupStageEvent; // void setupStage();
    NotifyEvent sofEvent; // void sof();
    NotifyEvent resetEvent; // void reset();
    NotifyEvent suspendEvent; // void suspend();
    NotifyEvent resumeEvent; // void resume();
    NotifyEvent isoInIncompleteEvent; // void isoInIncomplete();
    NotifyEvent isoOutIncompleteEvent; // void isoOutIncomplete();
    
    NotifyEvent devConnectedEvent; // void devConnected();
    NotifyEvent devDisconnectedEvent; //void devDisconnected();
    
    // high level access functions
    void ctlSendData(const ByteArray &ba);
    void ctlSendData(void *pbuf, unsigned short len);
    void ctlContinueSendData(void *pbuf, unsigned short len);
    void ctlError(uint8_t ep_direction);
    void ctlSendStatus();
    void ctlReceiveStatus();
    void ctlReceiveData(void *pbuf, unsigned short len);
    void ctlContinueRx(void *pbuf, unsigned short len);
};

};

#endif