#include "xpt2046.h"

XPT2046::XPT2046(Spi *spi, Gpio::PinName cs, Gpio::PinName penirq) :
    m_spi(spi),
    m_sensibility(2)
{
    m_cs = new Gpio(cs, Gpio::Output);
    m_cs->write(1);
    m_penirq = new Gpio(penirq);
//    m_busyPin = new Gpio(busy);

//    m_spi->setMasterMode();
//    m_spi->setCPOL_CPHA(0, 0);
//    m_spi->setBaudratePrescaler(1);
//    m_spi->setDataSize(16);

    setCalibration(340, 510, -360, -530);

    stmApp()->registerTaskEvent(EVENT(&XPT2046::task));
}

void XPT2046::setSensibility(int value)
{
    m_sensibility = BOUND(0, value, 4);
}

void XPT2046::task()
{
    if (!m_penirq->read())
    {
        m_spi->setBaudratePrescaler(4);
        m_cs->write(0);
        m_spi->write16(0xd0);
//        while (m_busyPin->read());
        int xp = m_spi->write16(0x90);
//        yp = m_spi->write16(0x00);
        int yp = m_spi->write16(0xB0);
        int z1 = m_spi->write16(0xC0);
        int z2 = m_spi->write16(0x00);
        m_cs->write(1);
        m_spi->setBaudratePrescaler(1);

        int press = 4 * 4096 * z1 / (xp / 16 * (z2 - z1));

        if (press > m_sensibility)
        {
//            if (!m_pressure)
//            {
//                m_xp = xp;
//                m_yp = yp;
//            }

            m_pressure = press - m_sensibility;

            TouchEvent::Type type = TouchEvent::Move;
            if (m_x < 0)
            {
                type = TouchEvent::Press;
                m_xp = xp;
                m_yp = yp;
            }

            int dx = xp - m_xp;
            int dy = yp - m_yp;
            if (dx > 1000 || dx < -1000 || dy > 1000 || dy < -1000)
                dx = dy = 0;

            // LPF
            m_xp += dx / 16;
            m_yp += dy / 16;

            int tx = m_xOffset + m_xp * m_xFactor / 32768;
            int ty = m_yOffset + m_yp * m_yFactor / 32768;

            //! @todo Orientation fix here

            m_x = tx;
            m_y = ty;

            touchEvent(type);
        }
        else
        {
            m_pressure = 0;
        }
    }
    else if (m_x >= 0) // was touched
    {
        touchEvent(TouchEvent::Release);
        m_x = m_y = -1;
        m_pressure = 0;
        m_xp = m_yp = 0;
    }
}