#include "max31855.h"

MAX31855::MAX31855(Spi *spi, Gpio::PinName cs) :
    m_spi(spi)
{
    m_cs = new Gpio(cs, Gpio::Output);
    m_cs->write(1);

//    m_spi->setBaudrate(2000000); // max 5 MHz
    m_spi->setBaudratePrescaler(5);
    m_spi->setCPOL_CPHA(0, 0);
    m_spi->setDataSize(16);
    m_spi->setMasterMode();
    m_spi->open();
}

void MAX31855::read()
{
    uint32_t v;
    m_cs->write(0);
    v = m_spi->read16() << 16;
    v |= m_spi->read16();
    m_cs->write(1);
    m_value = v;
}