#include "as5047d.h"

AS5047D::AS5047D(Spi *spi, Gpio::PinName csPin) :
    mSpi(spi)
{
    mCs = new Gpio(csPin);
    mCs->setAsOutput();
    mCs->write(1);
  
    Spi::Config conf;
    conf.CPHA = 1;
    conf.CPOL = 0;
    conf.master = 1;
    conf.baudrate = 3; // clock / 16
    spi->setConfig(conf);
    spi->setDataSize(16);
}

unsigned short AS5047D::read()
{
    unsigned short result;
    mCs->write(0);
//    for (int i=0; i<10; i++);
    result = mSpi->read16() << 2;
    mCs->write(1);
    return result;
}