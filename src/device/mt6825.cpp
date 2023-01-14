#include "mt6825.h"

MT6825::MT6825(Spi *spi, Gpio::PinName csPin, Gpio *pinMux) :
    AbsoluteEncoder(18),
    m_spi(spi),
    m_mux(pinMux)
{  
    if (!spi->isOpen())
    {
        Spi::Config conf;
        conf.CPHA = 1;
        conf.CPOL = 1;
        conf.master = 1;
        conf.baudrate = 1;
        conf.frame16bit = 1;
        spi->setConfig(conf);
        spi->open();
    }
  
    m_cs = new Gpio(csPin);
    m_cs->setAsOutput();
    m_cs->write(1);
}

uint32_t MT6825::readValue()
{
    uint16_t buf[2];
    if (m_mux)
        m_mux->write(0);
    m_cs->write(0);
    buf[0] = m_spi->write(0x8300);
    buf[1] = m_spi->read();
    m_cs->write(1);
    uint32_t result = 0;
    result = (buf[0] & 0xFF) << 10;
    result |= (buf[1] & 0xFC00) >> 6;
    result |= (buf[1] & 0x00F0) >> 4;
    
    return result; // not 16 bit!
}