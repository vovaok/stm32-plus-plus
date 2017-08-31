#include "usbep.h"
#include "usbdev.h" // poka tak

using namespace Usb;

UsbEndpoint::UsbEndpoint(EndpointDirection epDir, TransferType transferType, unsigned short maxPacketSize, unsigned char pollInterval) :
  UsbNode(UsbNode::NodeTypeEndpoint),
  mPollInterval(pollInterval)
    //csEndpoint(0L)
{
    mDescriptor = new EndpointDescriptor;
    mDescriptor->setEndpointAddress((unsigned char)epDir);
    mDescriptor->setAttributes(transferType);
    mDescriptor->setMaxPacketSize(maxPacketSize);
    mDescriptor->setInterval(pollInterval);
    assignDescriptor(mDescriptor);
}
//---------------------------------------------------------------------------

void UsbEndpoint::init()
{
    if (!isIn())
        mRxBuffer.resize(mDescriptor->maxPacketSize());
    if (device())
    {
        device()->connectEndpoint(this);
        device()->driver()->epOpen(mDescriptor->endpointAddress(), mDescriptor->maxPacketSize(), mDescriptor->attributes());
        if (mRxBuffer.size()) // if this is OUT endpoint => prepare buffer for reception
            device()->driver()->epPrepareRx(mDescriptor->endpointAddress(), mRxBuffer.data(), mRxBuffer.size());
    }
    UsbNode::init();
}

void UsbEndpoint::deInit()
{
    if (!isIn())
        mRxBuffer.clear();
    if (device())
        device()->driver()->epClose(mDescriptor->endpointAddress());  
    device()->disconnectEndpoint(this);
    UsbNode::deInit();
}

void UsbEndpoint::dataOut(int size)
{
    if (mDataOutEvent)
    {
        ByteArray ba(mRxBuffer.data(), size);
        mDataOutEvent(ba);
    }
    if (device() && mRxBuffer.size())
        device()->driver()->epPrepareRx(mDescriptor->endpointAddress(), mRxBuffer.data(), mRxBuffer.size());
}

void UsbEndpoint::dataIn()
{
//    printf("dataIn\n");
    if ((mDescriptor->attributes() & 0x3) == TransferInterrupt)
    {
        /* Ensure that the FIFO is empty before a new transfer, this condition could 
        be caused by a new transfer before the end of the previous transfer */
        device()->driver()->epFlush(mDescriptor->endpointAddress());
    }
}

//void UsbEndpoint::getRxBuffer(ByteArray &data)
//{
//    data = mRxBuffer;
//}

void UsbEndpoint::sof()
{
    if (mTxBuffer.size())
    {
        if (isIn() && (device()->deviceStatus() == UsbOtgConfigured))
        {
//            printf("perform TX, count=%d\n", mTxBuffer.size());
            device()->driver()->epTx(mDescriptor->endpointAddress(), mTxBuffer.data(), mTxBuffer.size());
            mTxBuffer.resize(0);
        }
    }
}

void UsbEndpoint::sendData(const ByteArray &data)
{
    mTxBuffer.append(data);
//    printf("data buffered\n");
  
//    mTxBuffer = data;
//    printf("sendData\n");
//    if (isIn() && (device()->deviceStatus() == UsbOtgConfigured))
//    {
//        device()->driver()->epTx(mDescriptor->endpointAddress(), mTxBuffer.data(), mTxBuffer.size());
//        mTxBuffer.clear();
//    }
}
//---------------------------------------------------------------------------
