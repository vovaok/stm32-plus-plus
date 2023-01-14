#ifndef _MT6825_H
#define _MT6825_H

#include "spi.h"
#include "absoluteencoder.h"

class MT6825 : public AbsoluteEncoder
{
public:
    MT6825(Spi *spi, Gpio::PinName csPin, Gpio *pinMux = 0L);
  
protected:
    virtual uint32_t readValue();
    
private:
    Spi *m_spi;
    Gpio *m_cs;
    Gpio *m_mux;
};

#endif