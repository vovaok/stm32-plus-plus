#include "radioonbinterface.h"

#define RADIO_BUSY_TIMEOUT  5

using namespace Objnet;

RadioOnbInterface::RadioOnbInterface(CC1200 *device) :
    cc1200(device),
    ledRx(0L), ledTx(0L),
    mTxBusy(false), mRxBusy(false),
    mRssi(0), mLqi(0),
    mCurTxMac(0),
    mHdBusyTimeout(0)
{
    mMaxFrameSize = 120;
    mCurHdr.size = 0;
#warning BusRadio temporary replaced with BusSwonb due to debug purposes
    mBusType = BusSwonb; // BusRadio
    stmApp()->registerTaskEvent(EVENT(&RadioOnbInterface::task));
    stmApp()->registerTickEvent(EVENT(&RadioOnbInterface::tick));
}
//---------------------------------------------------------------------------

static int errorCount = 0;

void RadioOnbInterface::task()
{
    CC1200::Status rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
    if (rfStatus == CC1200::CALIBRATE)
        return;
    
    mTxBusy = (rfStatus == CC1200::TX || rfStatus == CC1200::FSTXON);
    
    if (rfStatus == CC1200::RX_FIFO_ERROR)
        cc1200->sendCommand(CC1200::SFRX); // flush RX FIFO on error
    
    if (mCurTxMac && !mHdBusyTimeout)
    {
        if (nakEvent)
            nakEvent(mCurTxMac);
        mCurTxMac = 0;
    }
    
    if (ledTx)
        ledTx->setState(mTxBusy);  
    
    int sz = cc1200->getRxSize();
    if (sz)
    {
        mRxBusy = true;
        if (ledRx)
            ledRx->on();
    }
//    if (sz && ledRx)
//        ledRx->on();
    if (!mCurHdr.size && sz >= 2)
    {
        cc1200->read(reinterpret_cast<unsigned char *>(&mCurHdr), 2);
        sz -= 2;
    }
    if (mCurHdr.size && sz >= mCurHdr.size + 1)
    {
        if (mCurHdr.size < 5)
        {
            cc1200->sendCommand(CC1200::SIDLE);
            cc1200->sendCommand(CC1200::SFRX);
        }
        else
        {
            unsigned long id;
            CC1200::AppendedStatus sts;
            cc1200->read(reinterpret_cast<unsigned char *>(&id), 4);
            mCurRxMsg.setId(id);
//            if (id != 0x00800000)
//                errorCount = 0;
            int datasize = mCurHdr.size - 5;
            mCurRxMsg.data().resize(datasize);
            if (datasize)
                cc1200->read(reinterpret_cast<unsigned char*>(mCurRxMsg.data().data()), mCurRxMsg.size());
            cc1200->read(reinterpret_cast<unsigned char *>(&sts), 2);
            mRssi = sts.RSSI;
            mLqi = sts.LQI;
            bool ok = sts.CRC_OK;
            if (ok)
                writeRx(mCurRxMsg); 
            else
                errorCount++;
        }
        mCurHdr.size = 0;
        rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
        mRxBusy = false;
        mHdBusyTimeout = 0;
        mCurTxMac = 0;
    }
    else
    {
//        if (ledRx)
//            ledRx->off();
    }
    
    if (ledRx)
        ledRx->setState(mRxBusy);
    
    bool sendflag = false;
    if (!mHdBusyTimeout && !mTxBusy)
    {
        if (mUnsendBuffer.size())
        {
            if (trySend(mUnsendBuffer))
            {
                mUnsendBuffer.resize(0);
                mHdBusyTimeout = RADIO_BUSY_TIMEOUT;
                sendflag = true;
            }
        }
        else if (readTx(mCurTxMsg))
        {
            ByteArray ba;
            unsigned long id = mCurTxMsg.rawId();
            unsigned char mac = mCurTxMsg.isGlobal()? 0: (0x10 | mCurTxMsg.localId().mac);
            mCurTxMac = mac & 0x0F;
            ba.append(mac);
            ba.append(reinterpret_cast<const char*>(&id), 4);
            ba.append(reinterpret_cast<unsigned char *>(mCurTxMsg.data().data()), mCurTxMsg.size());
            mUnsendBuffer = ba;
            
            if (trySend(mUnsendBuffer))
            {
                mUnsendBuffer.resize(0);
                sendflag = true;
                if (id & 0x10000000) // in half-duplex mode: if message is local => wait response
                    mHdBusyTimeout = RADIO_BUSY_TIMEOUT;
            }
        }
    }
    
    if (!sendflag && (rfStatus == CC1200::IDLE))
        cc1200->startRx();
}

void RadioOnbInterface::tick(int dt)
{
    if (mHdBusyTimeout)
        --mHdBusyTimeout;
}
//---------------------------------------------------------------------------

bool RadioOnbInterface::trySend(ByteArray &ba)
{
    int avail = 127 - cc1200->readReg(CC1200_NUM_TXBYTES);
    if (avail < ba.size())
        return false;
    if (ledTx)
        ledTx->on();
    cc1200->send(reinterpret_cast<unsigned char *>(ba.data()), ba.size());
    return true;
}
//---------------------------------------------------------------------------

bool RadioOnbInterface::writeRx(const CommonMessage &msg)
{      
    if (mRxQueue.size() < mRxQueueSize) 
    {
        mRxQueue.push(msg);
        return true;
    }
    return false;
}

bool RadioOnbInterface::readTx(CommonMessage &msg)
{
    if (!mTxQueue.empty())
    {
        msg = mTxQueue.front();
        mTxQueue.pop();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

bool RadioOnbInterface::write(CommonMessage &msg)
{
    if (msg.size() > mMaxFrameSize)
        return false;
    if (mTxQueue.size() < mTxQueueSize) 
    {
        mTxQueue.push(msg);
        return true;
    }
    return false;
}

bool RadioOnbInterface::read(CommonMessage &msg)
{
    if (!mRxQueue.empty())
    {
        msg = mRxQueue.front();
        mRxQueue.pop();
        return true;
    }
    return false;
}

void RadioOnbInterface::flush()
{
    while (!mTxQueue.empty())
        mTxQueue.pop();
}
//---------------------------------------------------------------------------

int RadioOnbInterface::availableWriteCount()
{
    return mTxQueueSize - mTxQueue.size();
}
//---------------------------------------------------------------------------

int RadioOnbInterface::addFilter(unsigned long id, unsigned long mask)
{
#warning this function is shit, nado peredelat: vmesto CAN filter zapilit filter by address
    if (id & 0x10000000) // if local address
        cc1200->setAddress(0x10 | ((id >> 24) & 0xF));
    return 0;
}

void RadioOnbInterface::removeFilter(int number)
{
    
}
//---------------------------------------------------------------------------
//
//bool RadioOnbInterface::busPresent()
//{
//  
//}
////---------------------------------------------------------------------------

void RadioOnbInterface::setLeds(Led *rx, Led *tx)
{
    ledRx = rx;
    ledTx = tx;
    if (ledRx)
        ledRx->off();
    if (ledTx)
        ledTx->off();
}
//---------------------------------------------------------------------------
