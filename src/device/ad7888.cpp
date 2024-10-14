#include "ad7888.h"

AD7888::AD7888(Spi *spi, Gpio::PinName cs) :
    m_spi(spi),
    m_cs(new Gpio(cs, Gpio::Output))
{
    m_cs->write(1);

    m_spi->setCPOL_CPHA(1, 1);
    m_spi->setDataSize(16);
    m_spi->setBaudrate(1000000);
    m_spi->setMasterMode();
    m_spi->setTransferCompleteEvent(EVENT(&AD7888::sampleReady));
}

void AD7888::setSampleBuffer(uint16_t *buffer, int sampleCount)
{
    m_sampleCount = sampleCount;
    m_spi->setRxBuffer(reinterpret_cast<uint8_t *>(buffer), sampleCount * 2, true);
}

int AD7888::currentSampleCount() const
{
    return m_sampleCount - m_spi->dmaRx()->dataCounter();
}

void AD7888::addChannel(int number)
{
    m_sequence.append((char)((number & 7) << 3));
}

void AD7888::start()
{
    // m_sequence must contain at least one item!
    m_spi->open();
    m_cs->reset();
    m_spi->transferWordAsync(m_sequence[0]);
}

void AD7888::sampleReady(uint16_t value)
{
    m_cs->set();

    m_idx++;
    if (m_idx >= m_sequence.size())
        m_idx = 0;
    uint16_t word = m_sequence[m_idx] << 8;

    m_cs->reset();
    m_spi->transferWordAsync(word);
}