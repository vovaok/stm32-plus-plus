#include "shiftregister.h"

ShiftRegister::ShiftRegister(Gpio::PinName sck, Gpio::PinName miso, Gpio::PinName mosi, Gpio::PinName cs)
{
    m_sck = new Gpio(sck, Gpio::Output);
    m_miso = new Gpio(miso, Gpio::Input);
    m_mosi = new Gpio(mosi, Gpio::Output);
    m_cs = new Gpio(cs, Gpio::Output);
    m_cs->write(1);
}

ShiftRegister::ShiftRegister(Gpio* sck, Gpio* miso, Gpio* mosi, Gpio* cs) :
    m_sck(sck), m_miso(miso), m_mosi(mosi), m_cs(cs)
{
    m_sck->setAsOutput();
    m_miso->setAsInput();
    m_mosi->setAsOutput();
    m_cs->setAsOutput();
    m_cs->write(1);
}

uint32_t ShiftRegister::read(int bits)
{
    uint32_t data = 0;
    m_cs->write(1);
    m_cs->write(0);
    m_mosi->write(1);
    while (bits--)
    {
        data <<= 1;
        m_sck->reset();
        if (m_miso->read())
            data |= 1;
        else
            data &= ~1;
        m_sck->set();
    }
    m_mosi->write(0);
    m_cs->write(1);
    return data;
}

void ShiftRegister::write(uint32_t data, int bits)
{
    m_cs->write(0);
    uint32_t mask = 1 << (bits - 1);
    while (mask)
    {
        m_sck->write(0);
        if (data & mask)
            m_mosi->set();
        else
            m_mosi->reset();
        m_sck->write(1);
        mask >>= 1;
    }
    m_sck->write(0);
    m_cs->write(1);
}
