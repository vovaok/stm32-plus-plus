#include "modbus485.h"
#include "core/application.h"

using namespace Modbus;

Modbus485::Modbus485(Device *dev) :
    m_dev(dev),
    m_timeout(0)
{
    m_buffer.resize(255);
    m_outBuffer.resize(255);
    m_buffer.resize(0);
    m_outBuffer.resize(0);
    m_dev->open(Device::ReadWrite);

    stmApp()->registerTaskEvent(EVENT(&Modbus485::task));
    stmApp()->registerTickEvent(EVENT(&Modbus485::tick));
}

void Modbus485::task()
{
    int sz = m_dev->read(m_buffer.data() + m_buffer.size(), m_buffer.capacity() - m_buffer.size());
    m_buffer.resize(m_buffer.size() + sz);
    if (sz)
    {
        m_timeout = 10;
        responseUpdated();
    }

    if (!m_timeout && m_buffer.size())
    {
        if (m_buffer.size() > 4)
        {
            int crc_pos = m_buffer.size() - 2;
            uint16_t crc = crc16((uint8_t*)m_buffer.data(), crc_pos);
            if (crc == *reinterpret_cast<uint16_t*>(m_buffer.data() + crc_pos))
            {
                ADU adu;
                adu.addr = m_buffer[0];
                adu.func = m_buffer[1];
                adu.size = m_buffer.size() - 4;
                adu.data = m_buffer.data() + 2;
//                parseADU(adu);
                receiveADU(adu);
            }
        }

        m_buffer.resize(0);
    }
}

void Modbus485::tick(int dt)
{
    if (m_timeout > dt)
        m_timeout -= dt;
    else
        m_timeout = 0;
}

void Modbus485::writeADU(const ADU &adu)
{
    m_outBuffer.resize(0);
    m_outBuffer.append(adu.addr);
    m_outBuffer.append((unsigned char)adu.func);
    m_outBuffer.append(adu.data, adu.size);
    uint16_t crc = crc16((uint8_t*)m_outBuffer.data(), m_outBuffer.size());
    m_outBuffer.append(crc & 0xFF);
    m_outBuffer.append(crc >> 8);
    m_dev->write(m_outBuffer);
}


