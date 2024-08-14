#include "uartonbinterface.h"
#include "serial/serialframe.h"

#define SWONB_BUSY_TIMEOUT  2

using namespace Objnet;

UartOnbInterface::UartOnbInterface(Device *serialInterface) :
    mCurTxMac(0),
    mHdBusyTimeout(0)
{
    if (serialInterface->isSequential())
        m_device = new SerialFrame(serialInterface);
    else
        m_device = serialInterface;

    mMaxFrameSize = 64;
    mBuffer.resize(mMaxFrameSize + 4 - 1);
    m_sendBuffer = new char[mMaxFrameSize + 4];
    mBusType = BusWifi;
    if (serialInterface->isHalfDuplex())
        mBusType = BusSwonb;
    stmApp()->registerTaskEvent(EVENT(&UartOnbInterface::task));
    stmApp()->registerTickEvent(EVENT(&UartOnbInterface::tick));
    m_device->onReadyRead = EVENT(&UartOnbInterface::task);
    m_device->open(Device::ReadWrite);
}
//---------------------------------------------------------------------------

UartOnbInterface::~UartOnbInterface()
{
    delete m_sendBuffer;
}

void UartOnbInterface::task()
{
    if (m_busy)
        return;

    m_busy = true;

    int sz = m_device->read(mBuffer.data(), mBuffer.capacity());
    if (sz > 0)
    {
        mBuffer.resize(sz);
        msgReceived(mBuffer);
    }

    m_busy = false;
    
    if (mHdBusyTimeout < 0)
        mHdBusyTimeout = 0;

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
}

void UartOnbInterface::tick(int dt)
{
//    mWriteTimer += dt;
    if (isMaster && mHdBusyTimeout)
        --mHdBusyTimeout;
}
//---------------------------------------------------------------------------

__root static uint32_t ids[256];
static uint8_t ids_idx = 0;

void UartOnbInterface::msgReceived(const ByteArray &ba)
{
    mCurTxMac = 0;

    mHdBusyTimeout = 0;

    uint32_t id = *reinterpret_cast<const uint32_t*>(ba.data());
    bool accept = false;
    if (!m_filterCount)
        accept = true;
    for (int i=0; i<m_filterCount; i++)
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
        ids[ids_idx++] = *reinterpret_cast<uint32_t *>(mBuffer.data());

        CommonMessage msg(ba);
        receive(std::move(msg));
    }
}

bool UartOnbInterface::send(const CommonMessage &msg)
{
    if (mHdBusyTimeout)
        return false;

    uint32_t id = msg.rawId();
    if (id & 0x10000000) // if msg is local
        mCurTxMac = (id >> 24) & 0xF;
    else
        mCurTxMac = 0;

    ids[ids_idx++] = id | 0x80000000;

    uint32_t *dst = reinterpret_cast<uint32_t*>(m_sendBuffer);
    *dst++ = id;
    int size = msg.data().size();
    int cnt = (size + 3) >> 2;
    const uint32_t *src = reinterpret_cast<const uint32_t*>(msg.data().data());
    while (cnt--)
        *dst++ = *src++;

    if (m_device->write(m_sendBuffer, size + 4) > 0)
    {
        if (!isMaster)
            mHdBusyTimeout = SWONB_BUSY_TIMEOUT;
        else if (m_device->isHalfDuplex() && (id & 0x10000000)) // in half-duplex mode: if message is local => wait response
            mHdBusyTimeout = SWONB_BUSY_TIMEOUT;
        return true;
    }

    return false;
}

int UartOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    Filter f = {id & 0x1FFFFFFF, mask & 0x1FFFFFFF};
    if (m_filterCount < 8)
        mFilters[m_filterCount++] = f;
    return m_filterCount;
}

void UartOnbInterface::removeFilter(int number)
{
    if (number >= 0 && number < m_filterCount)
    {
        for (int i=number; i<7; i++)
            mFilters[i] = mFilters[i+1];
        --m_filterCount;
    }
}
//---------------------------------------------------------------------------
