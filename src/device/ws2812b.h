#pragma once

#include "spi.h"

class WS2812B
{
public:
    WS2812B(Gpio::Config spi_mosi, int N=1);
    
    void setLedCount(int N);
    int ledCount() const {return m_size;}
    
    void setPixel(int x, uint32_t color);
    uint32_t pixel(int x);
    
    void update();

private:
    Spi *m_spi;
    int m_size;
    ByteArray m_bits;
};