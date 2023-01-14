#include "uartonbinterface.h"

#define SWONB_BUSY_TIMEOUT  2

using namespace Objnet;

UartOnbInterface::UartOnbInterface(Device *serialInterface) :
    m_device(serialInterface),
//    mWriteTimer(30),
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
    m_device->open(Device::ReadWrite);
}
//---------------------------------------------------------------------------

void UartOnbInterface::task()
{
    char buferok[16];
    int sz = m_device->read(buferok, 16);
    if (sz)
        decode(buferok, sz);
    
    if (mCurTxMac && !mHdBusyTimeout)
    {
//        if (m_device->getErrorCode() && errorEvent && !noSOF)
//        {
//            errorEvent(mCurTxMac, ErrorInterface);
//        }
        
        if (nakEvent)
            nakEvent(mCurTxMac);
        mCurTxMac = 0;
    }
    
//    if (!mHdBusyTimeout)
//    {
//        if (mUnsendBuffer.size())
//        {
//            if (m_device->write(mUnsendBuffer.data(), mUnsendBuffer.size()) > 0)
//            {
//                mUnsendBuffer.resize(0);
//                if (m_device->isHalfDuplex())
//                    mHdBusyTimeout = SWONB_BUSY_TIMEOUT;
//            }
//        }
//        
//        if (m_device->isHalfDuplex())
//            mWriteTimer = 30;
//        
//#warning something wrong!!!
////        if (mWriteTimer >= 30)
//        {
//            mWriteTimer = 0;
//            while (readTx(mCurTxMsg))
//            {
//                ByteArray ba;
//                unsigned long id = mCurTxMsg.id;
//                if (id & 0x10000000) // if msg is local
//                    mCurTxMac = (id >> 24) & 0xF;
//                else
//                    mCurTxMac = 0;
//                ba.append(reinterpret_cast<const char*>(&id), 4);
//                ba.append(mCurTxMsg.data, mCurTxMsg.size);
//                mUnsendBuffer = encode(ba);
//                
//                if (m_device->write(mUnsendBuffer.data(), mUnsendBuffer.size()) > 0)
//                {
//                    mUnsendBuffer.resize(0);
//                    if (m_device->isHalfDuplex() && (id & 0x10000000)) // in half-duplex mode: if message is local => wait response
//                    {
//                        mHdBusyTimeout = SWONB_BUSY_TIMEOUT;
//                        break;
//                    }
//                }
//                else
//                    break;
//            }
//        }
//    }
}

void UartOnbInterface::tick(int dt)
{
//    mWriteTimer += dt;
    if (mHdBusyTimeout)
        --mHdBusyTimeout;
}
//---------------------------------------------------------------------------

void UartOnbInterface::decode(const char *data, int size)
{
    for (int i=0; i<size; i++)
    {
        char byte = data[i];
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
}

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
        CommonMessage msg;
        msg.setId(*reinterpret_cast<const unsigned long*>(ba.data()));
        msg.setData(ba);
        msg.data().remove(0, 4);
        receive(msg);
    }
}

bool UartOnbInterface::send(const CommonMessage &msg)
{
    if (mHdBusyTimeout)
        return false;

    ByteArray ba;
    uint32_t id = msg.rawId();
    if (id & 0x10000000) // if msg is local
        mCurTxMac = (id >> 24) & 0xF;
    else
        mCurTxMac = 0;
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    ba = encode(ba);
        
    if (m_device->write(ba.data(), ba.size()) > 0)
    {
        if (m_device->isHalfDuplex() && (id & 0x10000000)) // in half-duplex mode: if message is local => wait response
            mHdBusyTimeout = SWONB_BUSY_TIMEOUT;
        return true;
    }
    
    return false;
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
