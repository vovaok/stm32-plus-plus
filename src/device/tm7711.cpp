#include "tm7711.h"

TM7711::TM7711(Gpio::PinName sck, Gpio::PinName out)
{
    m_sck = new Gpio(sck, Gpio::Output);
    m_out = new Gpio(out);
}

int TM7711::read()
{
    if (!m_out->read()) // not busy
    {
        struct {int32_t v: 24;};
        v = 0;
        for (int i=0; i<24; i++)
        {
            v <<= 1;
            m_sck->write(1);
            delay();
            if (m_out->read())
                v |= 1;
            m_sck->write(0);
            delay();
        }
        for (int i=0; i<3; i++)
        {
            m_sck->write(1);
            delay();
            m_sck->write(0);
            delay();
        }
        m_value = v;
        return v;
    }
    return m_value;
}

void TM7711::delay()
{
    for (int w=100; --w;);
}
