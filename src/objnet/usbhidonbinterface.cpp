#include "usbhidonbinterface.h"

using namespace Objnet;

UsbHidOnbInterface::UsbHidOnbInterface(UsbInterface *usbif)
{
    mSeqNo = 0;
    mMaxFrameSize = 8;
    mBusType = BusUsbHid;
    
    ByteArray hidReport;
    fillReportDescriptor(hidReport);
    mHidIf = new UsbHidInterface(hidReport);
//    mHidIf->onReportReceiveEvent = EVENT(&UsbHidOnbInterface::receiveHidBuffer);
    mHidIf->onGetReportEvent = EVENT(&UsbHidOnbInterface::onGetReport);
    mHidIf->onSetReportEvent = EVENT(&UsbHidOnbInterface::onSetReport);
    mHidIf->setUseMultipleReportId(true);
    mHidEpIn = new UsbEndpoint(EndpointIN, TransferInterrupt, 14, 1);
    mHidEpOut = new UsbEndpoint(EndpointOUT, TransferInterrupt, 14, 1);
    usbif->attachNode(mHidIf);
    usbif->attachNode(mHidEpIn);
    usbif->attachNode(mHidEpOut);
    
    mReportTimer = new Timer();
    mReportTimer->onTimeout = EVENT(&UsbHidOnbInterface::onTimer);
    mReportTimer->start(2); // polling interval
}
//---------------------------------------------------------------------------

ByteArray UsbHidOnbInterface::__createReportDescriptor(unsigned char id, unsigned char size)
{
    using namespace Hid;
    ByteArray ba;
    ba += ReportId(id);
    ba += UsageMinimum(0x0101);
    ba += UsageMaximum(0x0100 + size);
    ba += ReportCount(size);
    return ba;
}

ByteArray UsbHidOnbInterface::createInputReportDescriptor(unsigned char id, unsigned char size)
{
    using namespace Hid;
    ByteArray ba = __createReportDescriptor(id, size);
    ba += Input(Data | Variable | Absolute);
    return ba;
}

ByteArray UsbHidOnbInterface::createOutputReportDescriptor(unsigned char id, unsigned char size)
{
    using namespace Hid;
    ByteArray ba = __createReportDescriptor(id, size);
    ba += Output(Data | Variable | Absolute);
    return ba;
}

ByteArray UsbHidOnbInterface::createFeatureReportDescriptor(unsigned char id, unsigned char size)
{
    using namespace Hid;
    ByteArray ba = __createReportDescriptor(id, size);
    ba += Feature(Data | Variable | Absolute);
    return ba;
}
//---------------------------------------------------------------------------

void UsbHidOnbInterface::fillReportDescriptor(ByteArray &ba)
{
    using namespace Hid;
    ba += UsagePage(VendorDefined);
    ba += Usage(0x01);
    ba += Collection(Hid::Application);
    ba += ReportSize(8);
    ba += LogicalMinimum(0);
    ba += LogicalMaximum(255);
    // main report:
    ba += createInputReportDescriptor(0x23, 2);
    ba += createOutputReportDescriptor(0x23, 2);
    // features:
    ba += createFeatureReportDescriptor(0x01, 13);
    ba += EndCollection();
}
//---------------------------------------------------------------------------

void UsbHidOnbInterface::onTimer()
{
    ByteArray ba;
    ba.append(0x23);
    char qsize = mTxQueue.size();
    ba.append(qsize);
    ba.append(mSeqNo);
    mHidIf->sendReport(ba);
}

static int kakaka = 0;

void UsbHidOnbInterface::onSetReport(int reportId, ByteArray &ba)
{
    if (reportId == 0x01)
    {
        UsbHidOnbMessage msg = *reinterpret_cast<UsbHidOnbMessage*>(ba.data());
        kakaka++;
        writeRx(msg);
    }
}

void UsbHidOnbInterface::onGetReport(int reportId, ByteArray &ba)
{
    if (reportId == 0x01)
    {
        UsbHidOnbMessage msg;
        if (readTx(msg))
        {
            ba = ByteArray(&msg, sizeof(UsbHidOnbMessage));
        }
    }
}
//--------------------------------------------------------------------------

bool UsbHidOnbInterface::readRx(UsbHidOnbMessage &msg)
{
    if (!mRxQueue.empty())
    {
        msg = mRxQueue.front();
        mRxQueue.pop_front();
        return true;
    }
//    else if (mCan->receive(0, msg))
//    {
//        return true;
//    }
    return false;
}

bool UsbHidOnbInterface::writeRx(UsbHidOnbMessage &msg)
{      
    if (mRxQueue.size() < mRxQueueSize) 
    {
        mRxQueue.push_back(msg);
        return true;
    }
    return false;
}

bool UsbHidOnbInterface::readTx(UsbHidOnbMessage &msg)
{
    if (!mTxQueue.empty())
    {
        msg = mTxQueue.front();
        mTxQueue.pop_front();
        return true;
    }
    return false;
}

bool UsbHidOnbInterface::writeTx(UsbHidOnbMessage &msg)
{
//    if (mTxQueue.empty())
//    {
//        if (mCan->send(msg))
//        {
//            return true;
//        }
//    }
    if (mTxQueue.size() < mTxQueueSize) 
    {
        mTxQueue.push_back(msg);
        mSeqNo++;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

bool UsbHidOnbInterface::write(const Objnet::CommonMessage &msg)
{
    if (msg.data().size() > mMaxFrameSize)
        return false;
    UsbHidOnbMessage outMsg;
    outMsg.id = msg.rawId();
    outMsg.size = msg.data().size();
    for (int i=0; i<outMsg.size; i++)
        outMsg.data[i] = msg.data()[i];
    return writeTx(outMsg);
      
//    if (!usb->isOpen())
//        return false;
//    unsigned long id = msg.rawId();
//    unsigned char sz = msg.data().size();
//    QByteArray ba;
//    ba.resize(13);
//    *reinterpret_cast<unsigned long*>(ba.data()) = id;
//    ba[4] = sz;
//    for (int i=0; i<sz; i++)
//        ba[5+i] = msg.data()[i];
//    usb->setFeature(0x01, ba);
//    return true;
}

bool UsbHidOnbInterface::read(Objnet::CommonMessage &msg)
{
    UsbHidOnbMessage inMsg;
    if (!readRx(inMsg))
        return false;
    msg.setId(inMsg.id);
    msg.data().clear();
    msg.data().append(inMsg.data, inMsg.size);
    return true;
    
//    if (!usb->isOpen())
//        return false;
//    QByteArray ba1 = usb->read(1);
//    if (!ba1.size())
//        return false;
//    if (!ba1[0])
//        return false;
//
//    QByteArray ba;
//    bool success = usb->getFeature(0x01, ba);
//    if (success)
//    {
//        unsigned long id = *reinterpret_cast<unsigned long*>(ba.data());
//        msg.setId(id);
//        unsigned char sz = ba[4];
//        msg.setData(QByteArray(ba.data() + 5, sz));
//    }
//    return success;
}

void UsbHidOnbInterface::flush()
{
    mTxQueue.clear();
//    mSeqNo = 0;
}

int UsbHidOnbInterface::availableWriteCount()
{
    return mTxQueueSize - mTxQueue.size();
}

int UsbHidOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    //qDebug() << "[UsbHidOnbInterface]: Filter is not implemented. id=" << id << "mask=" << mask;
    return 0;
}

void UsbHidOnbInterface::removeFilter(int number)
{
    //qDebug() << "[UsbHidOnbInterface]: Filter is not implemented. number=" << number;
}
