#include "usbmsc.h"

//uint8_t MscBot::__MSC_BOT_Data[MSC_MEDIA_PACKET];

using namespace Usb;

UsbMscInterface::UsbMscInterface(UsbNode *parent) :
    UsbInterface(UsbClassMsc, MscScsiTransparent, MscBBB, "BBB"),
    mInEp(0L), mOutEp(0L),
    mAltSet(0),
    mMaxLun(0)
{
    parent->attachNode(this);
  
    mInEp = new UsbEndpoint(EndpointIN, TransferBulk, 64, 0);
    mOutEp = new UsbEndpoint(EndpointOUT, TransferBulk, 64, 0);
    
    attachNode(mInEp);
    attachNode(mOutEp);
}

void UsbMscInterface::setup(const UsbSetupReq &req)
{    
    switch (req.bmRequest & USB_REQ_TYPE_MASK)
    {
      case USB_REQ_TYPE_CLASS:  
        switch (req.bRequest)
        {
          case mscGML:
            if ((req.wValue  == 0) && (req.wLength == 1) && ((req.bmRequest & 0x80) == 0x80))
            {
                mMaxLun = 0;//USBD_STORAGE_fops->GetMaxLun();
                if (mMaxLun > 0)
                    device()->driver()->ctlSendData(&mMaxLun, req.wLength);
                else
                    device()->ctlError(req);
            }
            else
            {
                device()->ctlError(req);
            }
            break;
            
          case mscBOMSR:
            if ((req.wValue  == 0) && (req.wLength == 0) && ((req.bmRequest & 0x80) != 0x80))    
                (void)0;//MSC_BOT_Reset(pdev);
            else
                device()->ctlError(req);
            break;
            
          default:
            device()->ctlError(req);
        }
        break;

      case USB_REQ_TYPE_STANDARD:
        switch (req.bRequest)
        {
          case USB_REQ_GET_INTERFACE :
            device()->driver()->ctlSendData(&mAltSet, 1);
            break;

          case USB_REQ_SET_INTERFACE :
            mAltSet = (unsigned char)(req.wValue);
            break;
            
          case USB_REQ_CLEAR_FEATURE:
            printf("USB MSC clear feature accepted!\n");
//            // Flush the FIFO and Clear the stall status 
//            DCD_EP_Flush(pdev, (uint8_t)req->wIndex);
//            // Re-activate the EP     
//            DCD_EP_Close (pdev , (uint8_t)req->wIndex);
//            if((((uint8_t)req->wIndex) & 0x80) == 0x80)
//            {
//            DCD_EP_Open(pdev,
//                        ((uint8_t)req->wIndex),
//                        MSC_EPIN_SIZE,
//                        USB_OTG_EP_BULK);
//            }
//            else
//            {
//            DCD_EP_Open(pdev,
//                        ((uint8_t)req->wIndex),
//                        MSC_EPOUT_SIZE,
//                        USB_OTG_EP_BULK);
//            }
//            // Handle BOT error
//            MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
            break;
        }
        
      default:
        device()->ctlError(req);
    }
}

//void UsbMscInterface::sendData(const ByteArray &ba)
//{
//    mInEp->sendData(ba);
//}

void UsbMscInterface::sendDataLL(unsigned char *data, int size)
{
    mInEp->sendDataLL(data, size);
}

void UsbMscInterface::prepareRxLL(unsigned char *data, int size)
{
    mOutEp->prepareRxLL(data, size);
}
//---------------------------------------------------------------------------
