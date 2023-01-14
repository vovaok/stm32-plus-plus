#ifndef _AS5047D_H
#define _AS5047D_H

#include "spi.h"

class AS5047D
{
private:
    Spi *mSpi;
    Gpio *mCs;
    
public:
    AS5047D(Spi *spi, Gpio::PinName csPin);
    
    unsigned short read();
};

#endif