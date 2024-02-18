#pragma once

#include "spi.h"

class AD7888
{
public:
    AD7888(Spi *spi, Gpio::PinName cs);

    void setSampleBuffer(uint16_t *buffer, int sampleCount);
    int currentSampleCount() const;

    void addChannel(int number);

    void start();

private:
    Spi *m_spi;
    Gpio *m_cs;
    ByteArray m_sequence;
    uint8_t m_idx = 0;
    int m_sampleCount = 0;

    void sampleReady();
};