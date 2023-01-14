#ifndef _MT6818_H
#define _MT6818_H

#include "spi.h"
#include "absoluteencoder.h"

class MT6818 : public AbsoluteEncoder
{
public:
    MT6818(Spi *spi, Gpio::PinName csPin, Gpio *pinMux);
  
protected:
    virtual uint32_t readValue();
    
private:
    Spi *m_spi;
    Gpio *m_cs;
    Gpio *m_mux;
};

#endif