#include "ws2812b.h"

// reset bit count
#define PRE     40

WS2812B::WS2812B(Gpio::Config spi_mosi, int N)
    : m_size(N)
{
    m_spi = new Spi(Gpio::NoConfig, Gpio::NoConfig, spi_mosi);
    m_spi->setBaudrate(3'200'000);
    m_spi->setUseDmaTx(true);
    m_spi->open();
    
    m_bits = ByteArray(m_size * 12 + PRE, 0x88);
}

void WS2812B::setLedCount(int N)
{
    m_size = N;
    m_bits.resize(m_size * 12 + PRE);
}

void WS2812B::setPixel(int x, uint32_t color)
{
    if (x < 0 || x >= m_size)
        return;
    
    uint32_t grb = ((color & 0x0000FF00) << 16)   // G
                 | (color & 0x00FF0000)           // R
                 | ((color & 0x000000FF) << 8);   // B
    
    static constexpr uint8_t LUT[4] = {0x88, 0x8E, 0xE8, 0xEE};

    uint8_t *dst = reinterpret_cast<uint8_t *>(m_bits.data()) + x * 12 + PRE;
    
    for (int i=0; i<12; i++, grb<<=2)
        *dst++ = LUT[grb >> 30];
}

uint32_t WS2812B::pixel(int x)
{
//    if (x < 0 || x >= m_size)
//        return 0;
    
    uint32_t grb = 0;
    const uint8_t *src = reinterpret_cast<const uint8_t *>(m_bits.data()) + x * 12 + PRE;
    for (uint32_t mask=0x00800000; mask; mask>>=2)
    {
        uint8_t b = *src++;
        if (b & 0x60)
            grb |= mask;
        if (b & 0x06)
            grb |= mask >> 1;
    }
    
    return ((grb & 0x0000FF00) << 8)    // R
         | ((grb & 0x00FF0000) >> 8)    // G
         | ((grb & 0x000000FF));        // B
}

void WS2812B::update()
{
    m_spi->write((uint8_t *)m_bits.data(), m_bits.size());
}