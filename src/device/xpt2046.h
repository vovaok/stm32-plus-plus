#ifndef _XPT2046_H
#define _XPT2046_H

#include "spi.h"
#include "touchpanel.h"

class XPT2046 : public TouchPanel
{
public:
    XPT2046(Spi *spi, Gpio::PinName cs, Gpio::PinName penirq);

protected:
    Spi *m_spi;
    Gpio *m_cs, *m_penirq;

    virtual bool read() override;
};

#endif