#ifndef _USBMSC_H
#define _USBMSC_H

#include "../usbdesc.h"
#include "../usbep.h"
#include "../usbif.h"
#include "../usbcfg.h"

namespace Usb
{
  
typedef enum
{
    MscScsiNotReported  = 0x00, //!< SCSI command set not reported
    MscRBC              = 0x01, //!< Reduced Block Commands (RBC) T10 Project 1240-D
    MscMMC5             = 0x02, //!< MMC-5 (ATAPI)
    MscSFF8070i         = 0x03, //!< obsolete
    MscUFI              = 0x04, //!< USB floppy interface
    MscQIC157           = 0x05, //!< obsolete
    MscScsiTransparent  = 0x06, //!< SCSI transparent command set
    MscLockable         = 0x07, //!< Lockable Mass Storage
    MscIEEE1667         = 0x08 //!< IEEE1667
} MscSubclass;

typedef enum
{
    MscCBI_CC       = 0x00, //!< Control/Bulk/Interrupt protocol with command completion interrupt
    MscCBI_NCC      = 0x01, //!< Control/Bulk/Interrupt protocol with no command completion interrupt
    MscBBB          = 0x50  //!< Bulk-Only Transport
} MscProtocol;

typedef enum
{
    mscADSC     = 0x00, //!< Accept Device-Specific Command (for CBI)
    mscGet      = 0xFC, //!< Get requests (for Lockable)
    mscPut      = 0xFD, //!< Put requests (for Lockable)
    mscGML      = 0xFE, //!< Get Max LUN (for BBB)
    mscBOMSR    = 0xFF  //!< Bulk-Only Mass Storage Reset (for BBB)
} MscRequest;
//---------------------------------------------------------------------------

class UsbMscInterface : public UsbInterface
{
private:
    UsbEndpoint *mInEp, *mOutEp;
    unsigned char mAltSet;
    unsigned char mMaxLun;
  
protected:
    void setup(const UsbSetupReq &req);
  
//    virtual void receiveData(const ByteArray &ba);   
    
public:
    UsbMscInterface();
//    void sendData(const ByteArray &ba);
    void setReceiveEvent(Closure<void(const ByteArray&)> e) {mOutEp->setDataOutEvent(e);}
    void setDataInEvent(Closure<void(void)> e) {mInEp->setDataInEvent(e);}
    
    void sendDataLL(unsigned char *data, int size);
};

};

#endif