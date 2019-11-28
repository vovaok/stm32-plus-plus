#ifndef _MSCBOT_H
#define _MSCBOT_H

#include <stdint.h>
#include <string.h>
#include "scsi.h"
#include "usbmsc.h"

#define MSC_MEDIA_PACKET             4096

// forward declaration:
class SCSI;

class MscBot
{
private:
    typedef enum
    {
        botStateNormal      = 0,
        botStateRecovery    = 1,
        botStateError       = 2
    } BotStatus;
  
    static uint8_t __MSC_BOT_Data[MSC_MEDIA_PACKET];
    Usb::UsbMscInterface *m_interface;
    SCSI *m_scsi;
    BotStatus m_status;
  
    void receiveData(const ByteArray &ba);
    void dataIn();
    
protected:
    void doSendData(unsigned char *data, int len);
    void prepareRx(unsigned char *data, int len);
    friend class SCSI;
    
    void setMaxLun(int lun);
    
public:
    typedef enum
    {
        botIdle         = 0,
        botDataOut      = 1,
        botDataIn       = 2,
        botLastDataIn   = 3,
        botSendData     = 4
    } State;
    
    typedef enum
    {
        BOT_CBW_SIGNATURE           = 0x43425355,
        BOT_CSW_SIGNATURE           = 0x53425355,
        BOT_CBW_LENGTH              = 31,
        BOT_CSW_LENGTH              = 13
    } BOT_DEFINES;
    
    typedef enum
    {
        cswCmdPassed        = 0x00,
        cswCmdFailed        = 0x01,
        cswPhaseError       = 0x02
    } CswStatus;

#pragma pack(push,1)
    typedef struct
    {
      uint32_t dSignature;
      uint32_t dTag;
      uint32_t dDataLength;
      uint8_t  bmFlags;
      uint8_t  bLUN;
      uint8_t  bCBLength;
      uint8_t  CB[16];
    } CBW;
    
    typedef struct 
    {
      uint32_t dSignature;
      uint32_t dTag;
      uint32_t dDataResidue;
      uint8_t  bStatus;
    } CSW;
#pragma pack(pop)

    MscBot(Usb::UsbMscInterface *iface, SCSI *scsi);
    
    uint8_t             *data;
    uint16_t            dataLen;
    uint8_t             state;
//    uint8_t             burstMode;
    CBW                 cbw;
    CSW                 csw;
    
    void cbwDecode();
    void sendData(unsigned char *data, int len);
    void sendCSW(MscBot::CswStatus status);
    void abort();
};

#endif