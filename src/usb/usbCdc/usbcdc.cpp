#include "usbcdc.h"

using namespace Usb;

UsbVcpCtrlInterface::UsbVcpCtrlInterface() :
    UsbInterface(UsbClassCdcControl, CdcCtrlSubclassACM, CdcCtrlProtocolCommonAT, "Control"),
    mCtrlEp(0L),
    cdcCmd(cdcNoCmd)
{
    attachNode(new CdcHeaderFuncNode);
    attachNode(new CdcCMFuncNode);
    attachNode(new CdcACMFuncNode);
    attachNode(new CdcUnionFuncNode);
    
    mCtrlEp = new UsbEndpoint(EndpointIN, TransferInterrupt, 8, 0xFF);
    attachNode(mCtrlEp);
    
    mLineCoding.baudrate = 57600;
    mLineCoding.stopbits = StopBits1;
    mLineCoding.parity = ParityEven;
    mLineCoding.wordlength = 8;
}

void UsbVcpCtrlInterface::setup(const UsbSetupReq &req)
{
    ByteArray buf;
    
//    printf("request: 0x%02X\n", req.bRequest);
    
    switch (req.bmRequest & USB_REQ_TYPE_MASK)
    {
      case USB_REQ_TYPE_CLASS:  
        if (req.wLength) // dummy handlers:
        {
            mCtlBuffer.resize(req.wLength);
            
            if (req.bmRequest & 0x80)
            {
                if (req.bRequest == cdcGetLineCoding)
                {
                    device()->driver()->ctlSendData(&mLineCoding, req.wLength);
                }
                else
                {                  
                    device()->driver()->ctlSendData(mCtlBuffer.data(), req.wLength);
                }
            }
            else
            {
                cdcCmd = req.bRequest;
                device()->driver()->ctlReceiveData(mCtlBuffer.data(), req.wLength);
            }
        }
        else
        {
            if (req.bRequest == cdcSetControlLineState)
            {
                mLineState = req.wValue;
//                printf("line state: %d\n", req.wValue);
            }
        }
        break;

      case USB_REQ_TYPE_STANDARD:
        switch (req.bRequest)
        {
          case USB_REQ_GET_DESCRIPTOR: 
            if ((req.wValue >> 8) == CDC_DESCRIPTOR_TYPE)
            {
                #ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
                readDescriptor(buf, false);   
                #else
                parent()->readDescriptor(buf);
                int len = parent()->descriptor()->length();
                buf = ByteArray(buf.data() + len, buf.size() - len);
                #endif 
            }
            if (req.wLength < buf.size())
                buf.resize(req.wLength);
            device()->driver()->ctlSendData(buf);
            break;

          case USB_REQ_GET_INTERFACE :
            device()->driver()->ctlSendData(&mAltSet, 1);
            break;

          case USB_REQ_SET_INTERFACE :
            mAltSet = (unsigned char)(req.wValue);
            break;
        }
        
      default:
        device()->ctlError(req);
    }
}

void UsbVcpCtrlInterface::ep0RxReady()
{
    if (cdcCmd == cdcSetLineCoding)
    {
        mLineCoding = *reinterpret_cast<LineCoding*>(mCtlBuffer.data());
    }
}
//---------------------------------------------------------------------------

UsbVcpDataInterface::UsbVcpDataInterface() :
    UsbInterface(UsbClassCdcData, 0, 0, "Data"),
    mInEp(0L), mOutEp(0L)
{
    mOutEp = new UsbEndpoint(EndpointOUT, TransferBulk, 64, 0);
    mInEp = new UsbEndpoint(EndpointIN, TransferBulk, 64, 0);
    
    mOutEp->setDataOutEvent(EVENT(&UsbVcpDataInterface::receiveData));
    
    attachNode(mOutEp);
    attachNode(mInEp);
}

void UsbVcpDataInterface::sendData(const ByteArray &ba)
{
    mInEp->sendData(ba);
}

void UsbVcpDataInterface::receiveData(const ByteArray &ba)
{
}

void UsbVcpDataInterface::setReceiveEvent(const ConstDataEvent &e)
{
    mOutEp->setDataOutEvent(e);
}
//---------------------------------------------------------------------------
