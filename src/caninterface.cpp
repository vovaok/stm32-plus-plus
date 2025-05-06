#include "caninterface.h"
#include "cansocket.h"

CanInterface::CanInterface(int fifoCount) :
    m_fifoCount(fifoCount)
{
    m_availFifo = (1 << fifoCount) - 1;
}

int CanInterface::acquireFifoChannel(CanSocket *socket)
{
    bool share = false;
    for (int i=0; i<m_fifoCount; i++)
    {
        if (!share && m_sharedFifo & (1 << i))
            share = true;
        if (m_availFifo & (1 << i))
        {
            m_availFifo &= ~(1 << i);
            if (share)
            {
                if (m_sockets[i])
                    m_sockets[i]->m_shareChannel = true;
                socket->m_shareChannel = true;
            }
            else
            {
                if (m_sockets.size() <= i)
                    m_sockets.resize(i + 1);
                m_sockets[i] = socket;
            }
            return i;
        }
    }

    // reset available FIFOs
    m_availFifo = (1 << m_fifoCount) - 1;
    // make the first FIFO shared (access by filter index only)
    m_sharedFifo |= 1;

    /*! @todo make resource counter for each FIFO instead of bit mask
              to properly release them later
    */

    return 0;
}

void CanInterface::releaseFifoChannel(int fifoChannel)
{
    if (fifoChannel < 0)
        return;
    m_availFifo |= (1 << fifoChannel);
    if (fifoChannel < m_sockets.size())
        m_sockets[fifoChannel] = nullptr;
}

void CanInterface::messageReceived(int fifoChannel)
{
    if (m_sharedFifo & (1 << fifoChannel))
    {
        int fidx = pendingMessageFilterIdx(fifoChannel);
        if (fmiSocketMap.count(fidx) && fmiSocketMap.at(fidx))
            fmiSocketMap.at(fidx)->onReadyRead();
    }
    else if (fifoChannel < m_sockets.size() && m_sockets[fifoChannel])
    {
        m_sockets[fifoChannel]->onReadyRead();
    }
}

uint8_t CanInterface::dlcFromSize(uint8_t size)
{
    if (size <= 8)
        return size;
    else if (size <= 12)
        return 9;
    else if (size <= 16)
        return 10;
    else if (size <= 20)
        return 11;
    else if (size <= 24)
        return 12;
    else if (size <= 32)
        return 13;
    else if (size <= 48)
        return 14;
    return 15;
}
