#include "mt6818.h"

MT6818::MT6818(Spi *spi, Gpio::PinName csPin, Gpio *pinMux) :
    AbsoluteEncoder(14),
    m_spi(spi),
    m_mux(pinMux)
{
    m_cs = new Gpio(csPin, Gpio::Output);
    m_cs->write(1);
  
//    Spi::Config conf;
//    conf.CPHA = 1;
//    conf.CPOL = 1;
//    conf.master = 1;
//    conf.baudrate = 1;
//    conf.frame16bit = 1;
//    spi->setConfig(conf);
}

uint32_t MT6818::readValue()
{
    uint16_t buf[2] = {0x8300, 0x8400};
    m_mux->write(1);
    for (int i=0; i<2; i++)
    {
        m_cs->write(0);
        buf[i] = m_spi->write(buf[i]);
        m_cs->write(1);
    }
    uint32_t result = 0;
    result = (buf[0] & 0xFF) << 6;
    result |= (buf[1] & 0xFC) >> 2;
    
    return result; // not 16 bit!
}