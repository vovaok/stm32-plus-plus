#include "mscbot.h"
#include "scsi.h"

uint8_t MscBot::__MSC_BOT_Data[MSC_MEDIA_PACKET];

MscBot::MscBot(Usb::UsbMscInterface *iface, SCSI *scsi) :
    m_interface(iface),
    m_scsi(scsi)
{
    data = __MSC_BOT_Data;
    memset(&cbw, 0L, sizeof(CBW));
    memset(&csw, 0L, sizeof(CSW));
    state = botIdle;
    m_status = botStateNormal;
    m_interface->setReceiveEvent(EVENT(&MscBot::receiveData));
    m_interface->setDataInEvent(EVENT(&MscBot::dataIn));
    scsi->msc = this;
}

void MscBot::dataIn()
{
    switch (state)
    {
      case botDataIn:
        if (m_scsi->processCmd(cbw.bLUN, cbw.CB) < 0)
        {
            sendCSW(cswCmdFailed);
        }
        break;

      case botSendData:
      case botLastDataIn:
        sendCSW(cswCmdPassed);
        break;

      default:
        break;
    }
}

void MscBot::receiveData(const ByteArray &ba)
{
    switch (state)
    {
      case botIdle:
        if (ba.size() == sizeof(CBW))
        {
            cbw = *reinterpret_cast<const CBW*>(ba.data());
            cbwDecode();
        }
        else
        {
            m_scsi->senseCode(cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
            m_status = botStateError;   
            abort();
        }
        break;

      case botDataOut:
        if (m_scsi->processCmd(cbw.bLUN, cbw.CB) < 0)
            sendCSW(cswCmdFailed);
        break;

      default:
        break;
    }
}

void MscBot::cbwDecode()
{
    csw.dTag = cbw.dTag;
    csw.dDataResidue = cbw.dDataLength;

    if (/*(USBD_GetRxCount (pdev ,MSC_OUT_EP) != sizeof(CBW)) ||*/
        (cbw.dSignature != BOT_CBW_SIGNATURE)||
        (cbw.bLUN > 1) || 
        (cbw.bCBLength < 1) || 
        (cbw.bCBLength > 16))
    {
        m_scsi->senseCode(cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
        m_status = botStateError;   
        abort();
    }
    else
    {
        if (m_scsi->processCmd(cbw.bLUN, cbw.CB) < 0)
        {
            abort();
        }
        /*Burst xfer handled internally*/
        else if ((state != botDataIn) && 
                (state != botDataOut) &&
                (state != botLastDataIn)) 
        {
            if (dataLen > 0)
            {
                sendData(data, dataLen);
            }
            else if (dataLen == 0) 
            {
                sendCSW(cswCmdPassed);
            }
        }
    }
}

void MscBot::sendData(unsigned char *data, int len)
{
    len = MIN(cbw.dDataLength, len);
    csw.dDataResidue -= len;
    csw.bStatus = cswCmdPassed;
    state = botSendData;
    m_interface->sendDataLL(data, len);
}

void MscBot::doSendData(unsigned char *data, int len)
{
    m_interface->sendDataLL(data, len);
}

void MscBot::prepareRx(unsigned char *data, int len)
{
    m_interface->prepareRxLL(data, len);
}

void MscBot::sendCSW(CswStatus status)
{
    csw.dSignature = BOT_CSW_SIGNATURE;
    csw.bStatus = status;
    state = botIdle;
    m_interface->sendDataLL(reinterpret_cast<unsigned char*>(&csw), sizeof(CSW));
}

void MscBot::abort()
{
    printf("MscBot abort()\n");
//    if ((cbw.bmFlags == 0) && (cbw.dDataLength != 0) && (status == botStateNormal))
//    {
//        DCD_EP_Stall(pdev, MSC_OUT_EP );
//    }
//    DCD_EP_Stall(pdev, MSC_IN_EP);
  
    if (m_status == botStateError)
    {
        printf("BOT state error\n");
//        DCD_EP_PrepareRx (pdev,
//                      MSC_OUT_EP,
//                      (uint8_t *)&MSC_BOT_cbw, 
//                      BOT_CBW_LENGTH);    
    }
}