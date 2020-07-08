#include "radioonbinterface.h"

#define RADIO_BUSY_TIMEOUT  10

using namespace Objnet;

//Led *testLed = 0;

RadioOnbInterface::RadioOnbInterface(CC1200 *device) :
    cc1200(device),
    ledRx(0L), ledTx(0L),
    mTxBusy(false), mRxBusy(false),
//    mRssi(0), mLqi(0),
    mCurTxMac(0),
    mBurstRx(false), mBurstTx(false),
    mRxSize(0),
    mAddress(0),
    mRxHead(0), mRxTail(0),
    rfStatus(CC1200::IDLE),
    mState(stateInit),
    mHdBusyTimeout(0),
    errorCount(0)
{
//    testLed = new Led(Gpio::PA2);
    mMaxFrameSize = 120;
    mCurHdr.size = 0;
    mCurHdr.address = 0;
    mBusType = BusRadio;
    memset(mRssi, 0, sizeof(mRssi));
    stmApp()->registerTaskEvent(EVENT(&RadioOnbInterface::task));
    stmApp()->registerTickEvent(EVENT(&RadioOnbInterface::tick));
    
    device->setRxTxEvent(EVENT(&RadioOnbInterface::onRxTxInterrupt));
}
//---------------------------------------------------------------------------

void RadioOnbInterface::tick(int dt)
{
    if (mHdBusyTimeout)
        --mHdBusyTimeout;
    
    if(mTimeOutNode)
      --mTimeOutNode;
    
     if(mTimeOutMaster)
      --mTimeOutMaster;
}
//---------------------------------------------------------------------------

static unsigned char txSize = 0;

int unsentBytes = 0;

static CC1200::Status oldRfStatus;

CC1200::Status statuses[256];
unsigned char stscnt = 0;

int packetsSent = 0;
int bytesAvail = 0;
int pktcnt = 0;
int recPhy = 0;
int recLog = 0;
int irqcnt = 0;
bool wasnak = false;

int flushes =0;

void RadioOnbInterface::task()
{
//    testLed->on();
    if (mState == stateInit)
    {
        rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
        oldRfStatus = rfStatus;
        cc1200->sendCommand(CC1200::SRX);
        mState = stateCalibrate;
//        rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
//        if (rfStatus == CC1200::CALIBRATE)
//            mState = stateCalibrate;
    }
    else if (mState == stateCalibrate)
    {
        rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
        if (rfStatus == CC1200::RX)
            mState = stateIdle;
    }
    
//    rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
//    if (rfStatus != oldRfStatus)
//        statuses[stscnt++] = rfStatus;
//    oldRfStatus = rfStatus;
    
//    bool flag = cc1200->getRxTxFlag();
    
    if (rfStatus == CC1200::TX_FIFO_ERROR)
    {
        flushes++;
        cc1200->sendCommand(CC1200::SFTX); // flush TX FIFO on error
    }
    
//    if (rfStatus == CC1200::RX_FIFO_ERROR)
//        cc1200->sendCommand(CC1200::SFRX); // flush RX FIFO on error
    
//    unsentBytes = cc1200->readReg(CC1200_NUM_TXBYTES);
//    mTxBusy = unsentBytes;
//    if (unsentBytes)
//        cc1200->send(0L, 0);
    
//    mTxBusy = (mState == stateTx);
    
    mRxSize = (mRxHead - mRxTail + mRxBufferSize) % mRxBufferSize; //cc1200->getRxSize();
    mRxBusy = mRxSize;
    
    pktcnt = packetsSent + recPhy;
    
//    if (flag)
//    {
//        rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
//        if (mTxBusy)
//        {
//            mTxBusy = false;
//        }
//        else
//        {
//            mRxSize = cc1200->getRxSize();
//            mRxBusy = mRxSize;
//        }
//    }
    
  
//    rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
//    if (rfStatus == CC1200::CALIBRATE)
//        return;

////    mTxBusy = (rfStatus == CC1200::TX || rfStatus == CC1200::FSTXON);
    
//    if (rfStatus == CC1200::RX_FIFO_ERROR)
//        cc1200->sendCommand(CC1200::SFRX); // flush RX FIFO on error
//    
//    if (rfStatus == CC1200::TX_FIFO_ERROR)
//    {
//        txSize = cc1200->readReg(CC1200_NUM_TXBYTES);
//        while (1)
//        {
//            ledTx->toggle();
//            for (int i=0; i<1000000; i++);
//        }
//        cc1200->sendCommand(CC1200::SFTX); // flush TX FIFO on error
//    }
    
//    mRxSize = cc1200->getRxSize();
//    mRxBusy = mRxSize;
    
    if (ledTx)
        ledTx->setState(mTxBusy);  
    if (ledRx)
        ledRx->setState(mRxBusy);
    
    if (isMaster)
        masterTask();
    else
        nodeTask();
    
//    rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));    
//    if (mState != stateTx && (rfStatus == CC1200::IDLE))
//        cc1200->startRx();
    
//    testLed->off();
}  

void RadioOnbInterface::onRxTxInterrupt()
{
    irqcnt++;
    rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
    if (mTxBusy)
    {
        mTxBusy = false;
        //return;
    }
    int sz = cc1200->getRxSize();
    if (!sz)
        return;
//    testLed->on();
    recPhy++;
    int readsz = MIN(sz, mRxBufferSize - mRxHead);
    cc1200->read(mRxBuffer + mRxHead, readsz);
    mRxHead += readsz;
    sz -= readsz;
    if (!sz)
    {
        //rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
//        testLed->off();
        return;
    }
    cc1200->read(mRxBuffer, sz);
    mRxHead = sz;
    //rfStatus = ((CC1200::Status)(cc1200->getStatus() & 0x70));
//    testLed->off();
}

void RadioOnbInterface::readRxBuffer(unsigned char *data, int size)
{
    while (mRxTail != mRxHead && size--)
    {
        *data++ = mRxBuffer[mRxTail++];
        if (mRxTail >= mRxBufferSize)
            mRxTail = 0;
    }
}
   
void RadioOnbInterface::prepareTxBuffer()
{
    mTxBuffer.resize(0); // clean the buffer without memory reallocating
    unsigned long id = mCurTxMsg.rawId() & 0x1FFFFFFF; // most significant 3 bits are not used in ONB
    unsigned char mac = mCurTxMsg.isGlobal()? 0: (0x10 | mCurTxMsg.localId().mac);
    
    // check if next message intended for the same address
    mBurstTx = false;
    if (!mTxQueue.empty())
    {
        CommonMessage &nextMsg = mTxQueue.front();
        unsigned char nextmac = 0x10 | nextMsg.localId().mac;
        mBurstTx = (mac == nextmac) && mac;
    }
    
    if (mBurstTx)
        id |= 0x80000000; // set the burst flag

    mCurTxMac = mac & 0x0F;
    mTxBuffer.append(mac);
    mTxBuffer.append(reinterpret_cast<const char*>(&id), 4);
    mTxBuffer.append(reinterpret_cast<unsigned char *>(mCurTxMsg.data().data()), mCurTxMsg.size());
    //mTxBuffer.append(mCurTxMsg.data());
}

unsigned long receivedId[256];
unsigned char recIdCnt = 0;
int lastHeadRx = 0;
int lastHeadTx = 0;

int packetsRcvd;
int packetsRcvd15;
int packetsSent15;
int packetsDiff15;

bool RadioOnbInterface::parseRxBuffer()
{
    bool packetReceived = false;
    if (!mCurHdr.size && mRxSize >= 2)
    {
        readRxBuffer(reinterpret_cast<unsigned char *>(&mCurHdr), 2);
        mRxSize -= 2;
        packetsRcvd++;
    }
    if (mCurHdr.size && mRxSize >= mCurHdr.size + 1)
    {
        if (mCurHdr.size < 5)
        {
//            cc1200->sendCommand(CC1200::SIDLE);
//            cc1200->sendCommand(CC1200::SFRX);
            errorCount++;
        }
        else
        {
            unsigned long id;
            CC1200::AppendedStatus sts;
            readRxBuffer(reinterpret_cast<unsigned char *>(&id), 4);
//            if (id & 0x00800000 && id != 0x108001F0)
                receivedId[recIdCnt++] = id;
            mBurstRx = id & (0x80000000);
            mCurRxMsg.setId(id & 0x1FFFFFFF);
            int datasize = mCurHdr.size - 5;
            mCurRxMsg.data().resize(datasize);
            if (datasize)
                readRxBuffer(reinterpret_cast<unsigned char*>(mCurRxMsg.data().data()), mCurRxMsg.size());
            readRxBuffer(reinterpret_cast<unsigned char*>(&sts), 2);
            mRxSize -= mCurHdr.size;
            mRssi[mCurTxMac] = sts.RSSI;
            //mLqi = sts.LQI;
            bool ok = sts.CRC_OK;
            if (ok)
            {
                writeRx(mCurRxMsg);
                packetsRcvd15++;
            }
            else
                errorCount++;
            packetReceived = true;
            recLog++;
        }
        mCurHdr.size = 0;
    }
    if (mCurHdr.size >= 128 || (mCurHdr.address && mCurHdr.address != mAddress))
    {
        // error
        errorCount++;
        mCurHdr.size = 0;
        mCurHdr.address = 0;
        mRxTail = mRxHead; // flush
//        cc1200->sendCommand(CC1200::SFRX); // flush RX FIFO on error
//        cc1200->sendCommand(CC1200::SIDLE);
//        cc1200->sendCommand(CC1200::SFRX);
    }
    return packetReceived;
}

void RadioOnbInterface::masterTask()
{
    switch (mState)
    {    
      case stateIdle:
        if (mRxSize)
        {
            errorCount++;
        }
        if (readTx(mCurTxMsg))
        {
            prepareTxBuffer();
            mState = stateTx;
        }
        break;
        
      case stateTx:
        if (mRxSize)
        {
            errorCount++;
        }
        if (mTxBusy)
        {
        
          if(!mTimeOutMaster)
          {          
            mTxBusy = false;
          }
         
        }
        else if (trySend(mTxBuffer))
        {
            mTxBuffer.resize(0);
            lastHeadTx = mRxHead;
            if (mCurTxMsg.isGlobal())
            {
                mState = stateIdle;
            }
            else if (mBurstTx) // && isMaster
            {
                readTx(mCurTxMsg);
                prepareTxBuffer();
            }
            else
            {
                mHdBusyTimeout = RADIO_BUSY_TIMEOUT;
                mState = stateRx;
            }
        }
        break;
        
      case stateRx:
        wasnak = false;
        if (!mHdBusyTimeout)
        {
            if (nakEvent && mCurTxMac)
                nakEvent(mCurTxMac);
            mRssi[mCurTxMac] = 0;
            mBurstRx = 0;
            mCurHdr.size = 0;
            mState = stateIdle;
            wasnak = true;
        }
        else if (parseRxBuffer())
        {
            if (mBurstRx)
                mHdBusyTimeout = 2 * RADIO_BUSY_TIMEOUT;
            else
            {
                mHdBusyTimeout = 0;
                mCurTxMac = 0;
                mRxBusy = false;
                mState = stateIdle;
                lastHeadRx = mRxHead;
            }
        }
        break;
    }
}

void RadioOnbInterface::nodeTask()
{
    switch (mState)
    {    
      case stateIdle:
        if (mRxSize)
        {
            mState = stateRx;
        }
        else if (mHdBusyTimeout) // can transmit
        {
            if (readTx(mCurTxMsg))
            {
                prepareTxBuffer();
                mState = stateTx;
            }
        }
        break;
        
      case stateTx:
        if (mTxBusy)
        {        
          if(!mTimeOutNode)         
            mTxBusy = false;      
        }
        else if (trySend(mTxBuffer))
        {
            mTxBuffer.resize(0);
            if (mCurTxMsg.isGlobal())
            {
                mHdBusyTimeout = 0;
                mState = stateIdle;
            }
            else if (mBurstTx)
            {
                readTx(mCurTxMsg);
                prepareTxBuffer();
            }
            else
            {
                mHdBusyTimeout = 0;
                mState = stateIdle;
                //mHdBusyTimeout = RADIO_BUSY_TIMEOUT;
                //mState = stateRx;
            }
        }
        break;
        
      case stateRx:
        if (parseRxBuffer())
        {
            if (mCurRxMsg.isGlobal())
            {
                mState = stateIdle;
            }
            else if (!mBurstRx)
            {
                mHdBusyTimeout = RADIO_BUSY_TIMEOUT;
                mState = stateIdle;
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------

int failSend;


bool RadioOnbInterface::trySend(ByteArray &ba)
{
    int txFifoCnt = cc1200->readReg(CC1200_NUM_TXBYTES);
    if (txFifoCnt)
    {
        failSend++;
        return false;
    }
    
    if (ledTx)
        ledTx->on();
    
    cc1200->send(reinterpret_cast<unsigned char*>(ba.data()), ba.size());
    packetsSent++;
    
    if (ba[0] == 0x1F)
    {
        packetsSent15++;
        packetsDiff15 = packetsSent15 - packetsRcvd15;
    }
    
    mTxBusy = true;
    mTimeOutMaster = 10;
    mTimeOutNode = 10;
    return true;
  
//    if (mTxBusy)
//        return false;
//    int avail = 126 - cc1200->readReg(CC1200_NUM_TXBYTES);
//    bytesAvail = avail;
//    if (avail < ba.size())
//        return false;
//    if (ledTx)
//        ledTx->on();
//    //cc1200->send(reinterpret_cast<unsigned char*>(ba.data()), ba.size());
//    cc1200->write(reinterpret_cast<unsigned char*>(ba.data()), ba.size());
//    packetsSent++;
//    bytesAvail = 126 - cc1200->readReg(CC1200_NUM_TXBYTES);
//    mTxBusy = true;
//    return true;
}
//---------------------------------------------------------------------------

bool _txread, _txwrite;

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
    _txread=true;
    if (!mTxQueue.empty())
    {
        msg = mTxQueue.front();
        if (_txwrite)
            while(1);
        mTxQueue.pop();
        if (_txwrite)
            while(1);
        _txread=false;
        return true;
    }
    _txread=false;
    return false;
}
//---------------------------------------------------------------------------

bool RadioOnbInterface::write(CommonMessage &msg)
{
    _txwrite = true;
    if (msg.size() > mMaxFrameSize)
    {
        _txwrite = false;
        return false;
    }
    if (mTxQueue.size() < mTxQueueSize) 
    {
        if (_txread)
            while(1);
        mTxQueue.push(msg);
        _txwrite = false;
        return true;
    }
    _txwrite = false;
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

int RadioOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
#warning this function is shit, nado peredelat: vmesto CAN filter zapilit filter by address
    if (id & 0x10000000) // if local address
    {
        mAddress = 0x10 | ((id >> 24) & 0xF);
        cc1200->setAddress(mAddress);
    }
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
