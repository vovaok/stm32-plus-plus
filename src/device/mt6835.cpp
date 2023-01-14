#include "mt6835.h"

MT6835::MT6835(Spi *spi, Gpio::PinName csPin) :
    AbsoluteEncoder(21),
    m_spi(spi)
{
    if (!spi->isOpen())
    {
        Spi::Config conf;
        conf.CPHA = 1;
        conf.CPOL = 1;
        conf.master = 1;
        conf.baudrate = 1;
        conf.frame16bit = 0;
        spi->setConfig(conf);
        spi->open();
    }
  
    m_cs = new Gpio(csPin);
    m_cs->setAsOutput();
    m_cs->write(1);
}

uint32_t MT6835::readValue()
{
    uint32_t result = 0;
    uint8_t buf[4];
    
    m_cs->write(0);
    m_spi->write(CmdBurstRead);
    m_spi->write(ANGLE1);
    for (int i=0; i<4; i++)
        buf[i] = m_spi->read();
    m_cs->write(1);
    
    result = (buf[0] << 13) | (buf[1] << 5) | (buf[2] >> 3);
//    StatusFlags status = buf[2] & 0x7;
//    uint8_t crc = buf[3];
    
    return result;
}

void MT6835::writeReg(Register reg, uint8_t value)
{
    m_cs->write(0);
    m_spi->write(CmdWrite | ((reg >> 8) & 0x0F));
    m_spi->write(reg & 0xFF);
    m_spi->write(value);
    m_cs->write(1);
}

uint8_t MT6835::readReg(Register reg)
{
    uint8_t result;
    m_cs->write(0);
    m_spi->write(CmdRead | ((reg >> 8) & 0x0F));
    m_spi->write(reg & 0xFF);
    result = m_spi->read();
    m_cs->write(1);
    return result;
}