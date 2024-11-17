#include "cansocket.h"

CanSocket::CanSocket(CanInterface *can, int flags) :
    m_can(can),
    m_flags(static_cast<CanInterface::Flags>(flags))
{
    m_fifoChannel = m_can->acquireFifoChannel(this);
    if (m_fifoChannel < 0)
        THROW(Exception::OutOfRange);
}
    
CanSocket::~CanSocket()
{
    m_can->releaseFifoChannel(m_fifoChannel);
}
    
int CanSocket::addFilter(uint32_t id, uint32_t mask)
{
    int filterIdx = m_can->configureFilter(m_flags, id, mask, m_fifoChannel);
    m_can->setRxInterruptEnabled(m_fifoChannel, true);
    /// @todo use filterIdx
    return filterIdx;
}

bool CanSocket::removeFilter(int index)
{
    return m_can->removeFilter(index);
}
    
int CanSocket::bytesAvailable() const
{
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
    /// @todo check filterIdx
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
    