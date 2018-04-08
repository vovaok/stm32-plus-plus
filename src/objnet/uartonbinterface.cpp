#include "uartonbinterface.h"

#define SWONB_BUSY_TIMEOUT  10

using namespace Objnet;

UartOnbInterface::UartOnbInterface(SerialInterface *serialInterface) :
    mInterface(serialInterface),
    mReadCnt(0),
    mWriteTimer(30),
    cs(0), esc(0), cmd_acc(0),
    mHdBusyTimeout(0)
{ 
    mMaxFrameSize = 64;
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
                cmd_acc = 1;
                break;

              case '}':
                if (cmd_acc)
                {
                    if (!cs && mBuffer.size())
                    {
                        mBuffer.resize(mBuffer.size()-1); // remove checksum
                        msgReceived(mBuffer);
                    }
                    cmd_acc = 0;
                }
                break;

              default:
                if (!cmd_acc)
                    break;
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
        
        if (mWriteTimer >= 30)
        {
            mWriteTimer = 0;
            while (readTx(mCurTxMsg))
            {
                ByteArray ba;
                unsigned long id = mCurTxMsg.id;
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
                    if (mInterface->isHalfDuplex())
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
    mHdBusyTimeout = 0;
    
    UartOnbMessage msg;
    msg.id = *reinterpret_cast<const unsigned long*>(ba.data());
    msg.size = ba.size() - 4;
    for (int i=0; i<msg.size; i++)
        msg.data[i] = ba[i+4];
    writeRx(msg);
    
//    unsigned long id = *reinterpret_cast<const unsigned long*>(ba.data());  
//    bool accept = false;
//    if (!mFilters.size())
//        accept = true;
//    foreach (Filter f, mFilters)
//    {
//        if ((f.id & f.mask) == (id & f.mask))
//        {
//            accept = true;
//            break;
//        }
//    }
//
//    if (accept)
//    {
//        CommonMessage msg;
//        msg.setId(id);
//        msg.setData(QByteArray(ba.data()+4, ba.size()));
//        mRxQueue << msg;
//    }
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

int UartOnbInterface::addFilter(unsigned long id, unsigned long mask)
{
    return 0;
}

void UartOnbInterface::removeFilter(int number)
{
   
}
//---------------------------------------------------------------------------
