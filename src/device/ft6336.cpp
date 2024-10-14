#include "ft6336.h"

FT6336::FT6336(I2c *i2c, Gpio::PinName pinRst, Gpio::PinName pinInt) :
    m_i2c(i2c)
{
    m_i2c->setBusClock(100000);
    m_i2c->open();
    
    m_dev = m_i2c->createDevice(m_address);
    
    m_rst = new Gpio(pinRst, Gpio::Output);
    m_int = new Gpio(pinInt, Gpio::pullUp);
    
    configure(); // may fail, check it later
    
    setSwappedXY(true);
    setCalibration(320, 0, -32768, 32768);
    
    m_timer.onTimeout = [this](){m_timerFlag = 1;};
    m_timer.start(16);
}

bool FT6336::read()
{
    uint8_t buf[6];
    
//    if (m_int->read())
//        return false;
    
    if (!m_timerFlag)
        return false;
    m_timerFlag = 0;
    
    // read number of active touch count
    if (m_dev->readReg(TD_STATUS, buf, 1))
    {
        m_pen = (buf[0] & 3) > 0;
        if (m_pen)
        {
            // first touch: P1_XH, second touch: P2_XH
            m_dev->readReg(P1_XH, buf, 6);
            {
                m_rawX = ((buf[0] & 0x0F) << 8) | buf[1];
                m_rawY = ((buf[2] & 0x0F) << 8) | buf[3];
                m_rawZ = m_sensibility + 1; //buf[4];
            }
        }
    }
    
    return true;
}

bool FT6336::configure()
{
    m_rst->write(0);
    for (int w=10000; --w;);
    m_rst->write(1);
    
    uint8_t buf[4];
    m_dev->readReg(ID_G_FOCALTECH_ID, buf, 1);
    if (buf[0] != 0x11)
        return false;
    
    m_dev->readReg(ID_G_CIPHER_MID, buf, 2);
    if (buf[0] != 0x26 || buf[1] > 2)
        return false;
    
    m_dev->readReg(ID_G_CIPHER_HIGH, buf, 1);
    if (buf[0] != 0x64)
        return false;
    
    return true;
}

