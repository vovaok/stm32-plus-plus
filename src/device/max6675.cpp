#include "max6675.h"

MAX6675::MAX6675(Spi *spi, Gpio::PinName cs) :
    m_spi(spi)
{
    m_cs = new Gpio(cs, Gpio::Output);
    m_cs->write(1);

//    m_spi->setBaudrate(2000000); // max 4 MHz
    m_spi->setBaudratePrescaler(5);
    m_spi->setCPOL_CPHA(0, 0);
    m_spi->setDataSize(16);
    m_spi->setMasterMode();
    m_spi->open();
    
//    m_timer.onTimeout = EVENT(&MAX6675::update);
//    m_timer.start(250);
}

void MAX6675::read()
{
    m_cs->write(0);
    uint16_t value = m_spi->read16();
    m_cs->write(1);
    m_temp = ((value >> 3) & 0xFFF) * 0.25f;
    m_fault = value & 0x0004;
}