#pragma once

#include "spi.h"
#include "core/timer.h"

class MAX6675
{
public:
    MAX6675(Spi *spi, Gpio::PinName cs);

    bool isFault() const {return m_fault;}
    const float &temperature() const {return m_temp;}
    void read();

private:
    Spi *m_spi;
    Gpio *m_cs;
    bool m_fault = false;
    float m_temp = 0;
//    Timer m_timer;
};