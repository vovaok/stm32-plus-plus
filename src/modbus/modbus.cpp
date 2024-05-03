#include "modbus.h"
#include "core/core.h"

namespace Modbus
{

uint16_t readWord(uint8_t const * &data)
{
    uint16_t w;
    reinterpret_cast<uint8_t*>(&w)[1] = *data++;
    reinterpret_cast<uint8_t*>(&w)[0] = *data++;
    return w;
}

void writeWord(uint8_t *&data, uint16_t word)
{
    *data++ = word >> 8;
    *data++ = (uint8_t)word;
}

}

using namespace Modbus;

ModbusBase::ModbusBase(Device *dev) :
    m_dev(dev),
    m_timeout(0)
{
    m_buffer.resize(255);
    m_outBuffer.resize(255);
    m_buffer.resize(0);
    m_outBuffer.resize(0);
    m_dev->open(Device::ReadWrite);

    stmApp()->registerTaskEvent(EVENT(&ModbusBase::task));
    stmApp()->registerTickEvent(EVENT(&ModbusBase::tick));
}

void ModbusBase::task()
{
    int sz = m_dev->read(m_buffer.data() + m_buffer.size(), m_buffer.capacity() - m_buffer.size());
    m_buffer.resize(m_buffer.size() + sz);
    if (sz)
    {
        m_timeout = 2;
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
                parseADU(adu);
            }
        }

        m_buffer.resize(0);
    }
}

void ModbusBase::tick(int dt)
{
    if (m_timeout > dt)
        m_timeout -= dt;
    else
        m_timeout = 0;
}

void ModbusBase::writeADU(const ADU &adu)
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

uint16_t ModbusBase::crc16(uint8_t *data, int size)
{
    uint16_t crc = 0xFFFF;
    for (int i=0; i<size; i++)
    {
        crc ^= (uint16_t)data[i];              // XOR byte into least sig. byte of crc
        for (int j=8; j; j--)
        {                                   // Loop over each bit
            if (crc & 0x0001)
            {                               // If the LSB is set
                crc >>= 1;                  // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else                            // Else LSB is not set
            {
                crc >>= 1;                  // Just shift right
            }
        }
    }
    return crc;
}
