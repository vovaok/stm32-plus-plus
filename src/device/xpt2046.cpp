#include "xpt2046.h"

XPT2046::XPT2046(Spi *spi, Gpio::PinName cs, Gpio::PinName penirq) :
    m_spi(spi)
{
    m_cs = new Gpio(cs, Gpio::Output);
    m_cs->write(1);
    m_penirq = new Gpio(penirq);
//    m_busyPin = new Gpio(busy);

//    m_spi->setMasterMode();
//    m_spi->setCPOL_CPHA(0, 0);
//    m_spi->setBaudratePrescaler(1);
//    m_spi->setDataSize(16);

    setCalibration(340, 510, -360, -530);
//    setCalibration(510, 340, -530, -360);
//    setSwappedXY(true);
}

bool XPT2046::read()
{
    m_pen = !m_penirq->read();

    m_spi->setBaudratePrescaler(4);
    m_cs->write(0);
    m_spi->write16(0xd0);
//        while (m_busyPin->read());
    int xp = m_spi->write16(0x90);
    int yp = m_spi->write16(0xB0);
    int z1 = m_spi->write16(0xC0);
    int z2 = m_spi->write16(0x00);
    m_cs->write(1);
    m_spi->setBaudratePrescaler(1);

    m_rawX = xp;
    m_rawY = yp;
    m_rawZ = 4 * 4096 * z1 / (xp / 16 * (z2 - z1));
    return true;
}