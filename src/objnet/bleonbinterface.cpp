#include "bleonbinterface.h"

#define SWONB_BUSY_TIMEOUT  2

using namespace Objnet;

BleOnbInterface::BleOnbInterface(SerialInterface *serialInterface) :
    mInterface(serialInterface),
    mReadCnt(0),
    mWriteTimer(30),
    mCurTxMac(0),
    cs(0), esc(0), cmd_acc(0), noSOF(0),
    mHdBusyTimeout(0)
{ 
    mMaxFrameSize = 64;
    mBusType = BusWifi;
    if (serialInterface->isHalfDuplex())
        mBusType = BusSwonb;
  //  stmApp()->registerTaskEvent(EVENT(&BleOnbInterface::task));
  //  stmApp()->registerTickEvent(EVENT(&BleOnbInterface::tick));
    mInterface->open(ReadWrite);
}
//---------------------------------------------------------------------------

void BleOnbInterface::task()
{
    ByteArray ba;
    if (mInterface->read(ba))
    {
//        char sbuf[10];
//        string s;
        for (int i=0; i<ba.size(); i++)
        {
            char byte = ba[i];
//            sprintf(sbuf, "0x%02X ", byte);
//            s += sbuf;
            switch (byte)
            {
              case '\\':
                esc = 1;
                break;

              case '{':
                mBuffer.clear();
                cs = 0;
                esc = 0;
                noSOF = 0;
//                if (cmd_acc && errorEvent)
//                    errorEvent(mCurTxMac, ErrorFrame); // not cur mac, but previous!!!!
                cmd_acc = 1;
                break;

              case '}':
                if (cmd_acc)
                {
                    if (!cs)
                    {
                        if (mBuffer.size())
                        {
                            mBuffer.resize(mBuffer.size()-1); // remove checksum
                            msgReceived(mBuffer);
                        }
                    }
                    else if (errorEvent)
                    {
                        errorEvent(mCurTxMac, ErrorChecksum);
                    }
                    cmd_acc = 0;
                }
                break;

              default:
                if (!cmd_acc)
                {
                    if (!noSOF && errorEvent)
                    {
                        noSOF = 1;
                        errorEvent(mCurTxMac, ErrorNoSOF);
                    }
                    break;
                }
                if (esc)
                    byte ^= 0x20;
                esc = 0;
                mBuffer.append(byte);
                cs += byte;
            }
        }
//        s += '\n';
//        printf(s.c_str());
    }
    
    if (mCurTxMac && !mHdBusyTimeout)
    {
        if (mInterface->getErrorCode() && errorEvent && !noSOF)
        {
            errorEvent(mCurTxMac, ErrorInterface);
        }
        
        if (nakEvent)
            nakEvent(mCurTxMac);
        mCurTxMac = 0;
    }
    
    if (!mHdBusyTimeout)
    {
        if (mUnsendBuffer.size())
        {
//            printf("unsend buffer write");
            if (mInterface->write(mUnsendBuffer) > 0)
            {
                mUnsendBuffer.resize(0);
                if (mInterface->isHalfDuplex())
                    mHdBusyTimeout = SWONB_BUSY_TIMEOUT;
            }
        }
        
        if (mInterface->isHalfDuplex())
            mWriteTimer = 30;
        
        if (mWriteTimer >= 30)
        {
            mWriteTimer = 0;
            while (readTx(mCurTxMsg))
            {
                ByteArray ba;
                unsigned long id = mCurTxMsg.id;
                if (id & 0x10000000) // if msg is local
                    mCurTxMac = (id >> 24) & 0xF;
                else
                    mCurTxMac = 0;
                ba.append(reinterpret_cast<const char*>(&id), 4);
                ba.append(mCurTxMsg.data, mCurTxMsg.size);
                mUnsendBuffer = encode(ba);
                
//                string s = ">> ";
//                char sbuf[10];
//                for (int i=0; i<mUnsendBuffer.size(); i++)
//                {
//                    sprintf(sbuf, "0x%02X ", mUnsendBuffer[i]);
//                    s += sbuf;
//                }
//                s += '\n';
//                printf(s.c_str());
                
                if (mInterface->write(mUnsendBuffer) > 0)
                {
                    mUnsendBuffer.resize(0);
                    if (mInterface->isHalfDuplex() && (id & 0x10000000)) // in half-duplex mode: if message is local => wait response
                    {
                        mHdBusyTimeout = SWONB_BUSY_TIMEOUT;
                        break;
                    }
                }
                else
                    break;
            }
        }
    }
}

void BleOnbInterface::tick(int dt)
{
    mWriteTimer += dt;
    if (mHdBusyTimeout)
        --mHdBusyTimeout;
}
//---------------------------------------------------------------------------

bool BleOnbInterface::readRx(BleOnbMessage &msg)
{
    if (!mRxQueue.empty())
    {
        msg = mRxQueue.front();
        mRxQueue.pop();
        return true;
    }
    return false;
}

bool BleOnbInterface::writeRx(BleOnbMessage &msg)
{      
    if (mRxQueue.size() < mRxQueueSize) 
    {
        mRxQueue.push(msg);
        return true;
    }
    printf("ONB RX full\n");
    return false;
}

bool BleOnbInterface::readTx(BleOnbMessage &msg)
{
    if (!mTxQueue.empty())
    {
        msg = mTxQueue.front();
        mTxQueue.pop();
        return true;
    }
    return false;
}

bool BleOnbInterface::writeTx(BleOnbMessage &msg)
{
    if (mTxQueue.size() < mTxQueueSize) 
    {
        mTxQueue.push(msg);
        return true;
    }
    printf("ONB TX full\n");
    return false;
}
//---------------------------------------------------------------------------

ByteArray BleOnbInterface::encode(const ByteArray &ba)
{
    ByteArray out;
    char cs = 0;
    out.append('{');
    int sz = ba.size();
    for (char i=0; i<=sz; i++)
    {
        char b = i<sz? ba[i]: cs;
        cs -= b;
        switch (b)
        {
            case '{':
            case '}':
            case '\\':
                out.append('\\');
                b ^= 0x20;
        }
        out.append(b);
    }
    out.append('}');
    return out;
}

void BleOnbInterface::msgReceived(const ByteArray &ba)
{
    mCurTxMac = 0;
  
    mHdBusyTimeout = 0;
    
    unsigned long id = *reinterpret_cast<const unsigned long*>(ba.data());  
    bool accept = false;
    if (!mFilters.size())
        accept = true;
    int fcnt = mFilters.size();
    for (int i=0; i<fcnt; i++)
    {
        Filter &f = mFilters[i];
        if ((f.id & f.mask) == (id & f.mask))
        {
            accept = true;
            break;
        }
    }
    
    if (accept)
    {    
        BleOnbMessage msg;
        msg.id = *reinterpret_cast<const unsigned long*>(ba.data());
        msg.size = ba.size() - 4;
        for (int i=0; i<msg.size; i++)
            msg.data[i] = ba[i+4];
        writeRx(msg);
    }
}
//---------------------------------------------------------------------------

bool BleOnbInterface::write(CommonMessage &msg)
{
    if (msg.data().size() > mMaxFrameSize)
        return false;
    BleOnbMessage outMsg;
    outMsg.id = msg.rawId();
    int sz = msg.data().size();
    outMsg.size = sz;
    for (int i=0; i<sz; i++)
        outMsg.data[i] = msg.data()[i];
    return writeTx(outMsg);
}

bool BleOnbInterface::read(CommonMessage &msg)
{
    BleOnbMessage inMsg;
    if (!readRx(inMsg))
        return false;
    msg.setId(inMsg.id);
    msg.data().clear();
    msg.data().append(inMsg.data, inMsg.size);
    return true;
}
//---------------------------------------------------------------------------

void BleOnbInterface::flush()
{
    while (!mTxQueue.empty())
        mTxQueue.pop();
}
//---------------------------------------------------------------------------

int BleOnbInterface::availableWriteCount()
{
    return mTxQueueSize - mTxQueue.size();
}

int BleOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    Filter f = {id & 0x1FFFFFFF, mask & 0x1FFFFFFF};
    mFilters.push_back(f);
    return mFilters.size();
}

void BleOnbInterface::removeFilter(int number)
{
    if (number >= 0 && number < mFilters.size())
        mFilters.erase(mFilters.begin() + number);
}
//---------------------------------------------------------------------------
