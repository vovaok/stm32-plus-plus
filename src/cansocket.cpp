#include "cansocket.h"

CanSocket::CanSocket(CanInterface *can, int flags) :
    m_can(can),
    m_flags(static_cast<CanInterface::Flags>(flags))
{
    m_fifoChannel = m_can->acquireFifoChannel(this);
}

CanSocket::~CanSocket()
{
    //! @todo remove appropriate m_can->fmiSocketMap entries!
    m_can->releaseFifoChannel(m_fifoChannel);
}

int CanSocket::addFilter(uint32_t id, uint32_t mask)
{
    int filterIdx = m_can->configureFilter(m_flags, id, mask, m_fifoChannel);
    if (filterIdx < 0)
        THROW(Exception::OutOfRange);
    m_can->setRxInterruptEnabled(m_fifoChannel, true);
    m_can->fmiSocketMap[filterIdx] = this;
    return filterIdx;
}

bool CanSocket::removeFilter(int index)
{
    if (m_can->fmiSocketMap.count(index) && m_can->fmiSocketMap.at(index) == this)
        m_can->fmiSocketMap.erase(index);
    return m_can->removeFilter(index);
}

int CanSocket::bytesAvailable() const
{
    if (m_shareChannel)
    {
        int fidx = m_can->pendingMessageFilterIdx(m_fifoChannel);
        if (!m_can->fmiSocketMap.count(fidx) || m_can->fmiSocketMap.at(fidx) != this)
            return -1;
    }
    int len = m_can->pendingMessageLength(m_fifoChannel);
    if (len < 0)
        return -1;
    if (m_flags & CanInterface::ExtId)
        return len + 4;
    else
        return len + 2;
}

int CanSocket::readData(char *data, int size)
{
    // check filter index
    if (m_shareChannel)
    {
        int fidx = m_can->pendingMessageFilterIdx(m_fifoChannel);
        if (!m_can->fmiSocketMap.count(fidx) || m_can->fmiSocketMap.at(fidx) != this)
            return -1;
    }

    if (m_flags & CanInterface::ExtId)
    {
        if (size < 4)
            return -1;
        int len = m_can->receiveMessage(reinterpret_cast<uint32_t*>(data),
                                        reinterpret_cast<uint8_t*>(data + 4),
                                        size - 4, m_fifoChannel);
        if (len < 0)
            return 0;
        return len + 4;
    }

    if (size < 2)
        return -1;
    int len = m_can->receiveMessage(reinterpret_cast<uint32_t*>(data),
                                    reinterpret_cast<uint8_t*>(data + 2),
                                    size - 2, m_fifoChannel);
    if (len < 0)
        return 0;
    return len + 2;
}

int CanSocket::writeData(const char *data, int size)
{
    bool r;
    if (m_flags & CanInterface::ExtId)
    {
        if (size < 4)
            return -1;
        r = m_can->transmitMessage(m_flags,
                                   *reinterpret_cast<const uint32_t*>(data),
                                   reinterpret_cast<const uint8_t*>(data) + 4,
                                   size - 4);
    }
    else
    {
        if (size < 2)
            return -1;
        r = m_can->transmitMessage(m_flags,
                                   *reinterpret_cast<const uint16_t*>(data),
                                   reinterpret_cast<const uint8_t*>(data) + 2,
                                   size - 2);
    }
    if (r)
        return size;
    return 0;
}
