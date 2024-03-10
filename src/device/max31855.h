#pragma once

#include "spi.h"

class MAX31855
{
public:
    MAX31855(Spi *spi, Gpio::PinName cs);

    void read();

    bool isFault() const {return m_value & 0x00010007;}
    float temperature() const {return (m_value >> 18) * 0.25f;}
    float internalTemp() const {return (static_cast<int16_t>(m_value) >> 4) * 0.0625f;}

private:
    Spi *m_spi;
    Gpio *m_cs;
    uint32_t m_value = 0xFFFFFFFF;
};