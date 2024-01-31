#ifndef _XPT2046_H
#define _XPT2046_H

#include "spi.h"
#include "gfx/touchscreen.h"

class XPT2046 : public TouchScreen
{
public:
    XPT2046(Spi *spi, Gpio::PinName cs, Gpio::PinName penirq);

protected:
    Spi *m_spi;
    Gpio *m_cs, *m_penirq;

    virtual bool read() override;
};

#endif