#include "uartonbinterface.h"

#define SWONB_BUSY_TIMEOUT  2

using namespace Objnet;

UartOnbInterface::UartOnbInterface(SerialInterface *serialInterface) :
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
    stmApp()->registerTaskEvent(EVENT(&UartOnbInterface::task));
    stmApp()->registerTickEvent(EVENT(&UartOnbInterface::tick));
    mInterface->open(ReadWrite);
}
//---------------------------------------------------------------------------

void UartOnbInterface::task()
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
        
//        if (mWriteTimer >= 30)
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

void UartOnbInterface::tick(int dt)
{
    mWriteTimer += dt;
    if (mHdBusyTimeout)
        --mHdBusyTimeout;
}
//---------------------------------------------------------------------------

bool UartOnbInterface::readRx(UartOnbMessage &msg)
{
    if (!mRxQueue.empty())
    {
        msg = mRxQueue.front();
        mRxQueue.pop();
        return true;
    }
    return false;
}

bool UartOnbInterface::writeRx(UartOnbMessage &msg)
{      
    if (mRxQueue.size() < mRxQueueSize) 
    {
        mRxQueue.push(msg);
        return true;
    }
    printf("ONB RX full\n");
    return false;
}

bool UartOnbInterface::readTx(UartOnbMessage &msg)
{
    if (!mTxQueue.empty())
    {
        msg = mTxQueue.front();
        mTxQueue.pop();
        return true;
    }
    return false;
}

bool UartOnbInterface::writeTx(UartOnbMessage &msg)
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

ByteArray UartOnbInterface::encode(const ByteArray &ba)
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

void UartOnbInterface::msgReceived(const ByteArray &ba)
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
        UartOnbMessage msg;
        msg.id = *reinterpret_cast<const unsigned long*>(ba.data());
        msg.size = ba.size() - 4;
        for (int i=0; i<msg.size; i++)
            msg.data[i] = ba[i+4];
        writeRx(msg);
    }
}
//---------------------------------------------------------------------------

bool UartOnbInterface::write(CommonMessage &msg)
{
    if (msg.data().size() > mMaxFrameSize)
        return false;
    UartOnbMessage outMsg;
    outMsg.id = msg.rawId();
    int sz = msg.data().size();
    outMsg.size = sz;
    for (int i=0; i<sz; i++)
        outMsg.data[i] = msg.data()[i];
    return writeTx(outMsg);
}

bool UartOnbInterface::read(CommonMessage &msg)
{
    UartOnbMessage inMsg;
    if (!readRx(inMsg))
        return false;
    msg.setId(inMsg.id);
    msg.data().clear();
    msg.data().append(inMsg.data, inMsg.size);
    return true;
}
//---------------------------------------------------------------------------

void UartOnbInterface::flush()
{
    while (!mTxQueue.empty())
        mTxQueue.pop();
}
//---------------------------------------------------------------------------

int UartOnbInterface::availableWriteCount()
{
    return mTxQueueSize - mTxQueue.size();
}

int UartOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    Filter f = {id & 0x1FFFFFFF, mask & 0x1FFFFFFF};
    mFilters.push_back(f);
    return mFilters.size();
}

void UartOnbInterface::removeFilter(int number)
{
    if (number >= 0 && number < mFilters.size())
        mFilters.erase(mFilters.begin() + number);
}
//---------------------------------------------------------------------------
