#include "adxl345.h"

ADXL345::ADXL345(I2c *i2c) :
    m_dev(i2c),
    m_valid(false)
{
    for (int i=0; i<3; i++)
        m_acc[i] = 0;
    m_dev->open();
    
    uint8_t devid = readReg(DEVID);
    if (devid == 0xe5)
    {
        writeReg(POWER_CTL, 0x00); // disable
        writeReg(BW_RATE, 0x0D); // 800 Hz output data rate
        writeReg(DATA_FORMAT, 0x08 | 0x03); // FULL RES on; +/-16g
        writeReg(POWER_CTL, 0x08); // enable Measure
        m_valid = true;
    }
}

uint8_t ADXL345::readReg(RegisterName reg)
{
    uint8_t buf = 0;
    m_dev->write(ADXL_I2C_ADDR, (uint8_t*)&reg, 1);
    m_dev->read(ADXL_I2C_ADDR, &buf, 1);
    return buf;
}

bool ADXL345::writeReg(RegisterName reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return m_dev->write(ADXL_I2C_ADDR, buf, 2);
}

bool ADXL345::readAcc()
{
    bool r = false;
    uint8_t buf[1] = {DATAX0};
    r = m_dev->write(ADXL_I2C_ADDR, buf, 1);
    if (!r)
        return false;
    r = m_dev->read(ADXL_I2C_ADDR, (uint8_t*)m_acc, 6);
    for (int i=0; i<3; i++)
        m_value[i] = m_acc[i] / 256.f;
    return r;
}