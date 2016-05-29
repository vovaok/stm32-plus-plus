#include "usbhid.h"

using namespace Usb;

UsbHidInterface::UsbHidInterface(const ByteArray &report) :
    UsbNode(UsbNode::NodeTypeCsInterface),
    mInEp(0L), mOutEp(0L),
    mProtocol(0),
    mIdleState(0),
    mMultipleReportId(false),
    mAltSet(0),
    mPollTimer(0)
{
    mDescriptor = new HidInterfaceDescriptor();
    setReport(report);
    assignDescriptor(mDescriptor);
}

void UsbHidInterface::setReport(const ByteArray &reportDescriptor)
{
    mReport = reportDescriptor;
    mDescriptor->setReportSize(reportDescriptor.size());
}
//---------------------------------------------------------------------------

void UsbHidInterface::addChild(UsbNode *child)
{
    UsbEndpoint *ep = dynamic_cast<UsbEndpoint*>(child);
    if (ep)
    {
        if (ep->isIn())
            mInEp = ep;
        else
        {
            mOutEp = ep;
            mOutEp->setDataOutEvent(EVENT(&UsbHidInterface::onReportReceive));
        }
    }
//    UsbNode::addChild(child);
}
//---------------------------------------------------------------------------

void UsbHidInterface::setup(const UsbSetupReq &req)
{
    ByteArray buf;
    
    switch (req.bmRequest & USB_REQ_TYPE_MASK)
    {
      case USB_REQ_TYPE_CLASS:  
        switch (req.bRequest)
        {          
          case HID_REQ_SET_PROTOCOL:
            mProtocol = (unsigned char)(req.wValue);
            break;
          
          case HID_REQ_GET_PROTOCOL:
            device()->driver()->ctlSendData(&mProtocol, 1);
            break;
          
          case HID_REQ_SET_IDLE:
            mIdleState = (unsigned char)(req.wValue >> 8);
            break;
                
          case HID_REQ_GET_IDLE:
            device()->driver()->ctlSendData(&mIdleState, 1);
            break;
            
          case HID_REQ_SET_REPORT:
            mReqBuffer = req;
            mCtlBuffer.resize(req.wLength);
            device()->driver()->ctlReceiveData(mCtlBuffer.data(), mCtlBuffer.size());
            break;
             
          case HID_REQ_GET_REPORT:
            if (onGetReportEvent)
            {
                unsigned char reportType = (req.wValue >> 8) & 0xFF;
                unsigned char reportId = req.wValue & 0xFF;
                if (reportType == HidReportTypeFeature && onGetReportEvent)
                {
                    onGetReportEvent(reportId, buf);
                    if (mMultipleReportId)
                    {
                        ByteArray temp;
                        temp.append(reportId);
                        temp.append(buf);
                        buf = temp;
                    }
                }
//                else if (reportType == HidReportTypeOutput && onReportRequestEvent)
//                {
//                    onReportRequestEvent();
//                }
                if (req.wLength < buf.size())
                    buf.resize(req.wLength);
                device()->driver()->ctlSendData(buf);
            }
            else
            {
                device()->ctlError(req);
            }
            break;
          
          default:
            device()->ctlError(req);
            return;
        }
        break;

      case USB_REQ_TYPE_STANDARD:
        switch (req.bRequest)
        {
          case USB_REQ_GET_DESCRIPTOR: 
            if (req.wValue >> 8 == HID_REPORT_DESC)
            {
                buf = mReport;
            }
            else if (req.wValue >> 8 == HID_DESCRIPTOR_TYPE)
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
    }
}

void UsbHidInterface::ep0RxReady()
{
    unsigned char reportType = (mReqBuffer.wValue >> 8) & 0xFF;
    unsigned char reportId = mReqBuffer.wValue & 0xFF;
    if (reportType == HidReportTypeFeature && onSetReportEvent)
    {
        if (mMultipleReportId)
            mCtlBuffer.remove(0, 1);
        onSetReportEvent(reportId, mCtlBuffer);
    }
    else if (reportType == HidReportTypeOutput && onReportReceiveEvent)
    {
        onReportReceiveEvent(mCtlBuffer);
    }
}

void UsbHidInterface::sof()
{
    mPollTimer++;
    if (mPollTimer >= mInEp->pollingInterval())
    {
        mPollTimer = 0;
        if (onReportRequestEvent)
            onReportRequestEvent();
    }
}
//---------------------------------------------------------------------------

void UsbHidInterface::sendReport(const ByteArray &report)
{
    mInEp->sendData(report);
}

void UsbHidInterface::onReportReceive(const ByteArray &report)
{
    if (onReportReceiveEvent)
        onReportReceiveEvent(report);
}
//---------------------------------------------------------------------------