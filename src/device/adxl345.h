#ifndef _ADXL345_H
#define _ADXL345_H

#include "i2c.h"

//#define ADXL_I2C_ADDR     0x1D // 0x3A/0x3B
#define ADXL_I2C_ADDR     0xA6 //0x53 // 0xA6/0xA7

class ADXL345
{
public:
    ADXL345(I2c *i2c);
    
public:
    typedef enum
    {
        DEVID = 0x00,
        THRESH_TAP = 0x1D,
        OFSX = 0x1E,
        OFSY = 0x1F,
        OFSZ = 0x20,
        DUR = 0x21,
        LATENT = 0x22,
        WINDOW = 0x23,
        THRESH_ACT = 0x24,
        THRESH_INACT = 0x25,
        TIME_INACT = 0x26,
        ACT_INACT_CTL = 0x27,
        THRESH_FF = 0x28,
        TIME_FF = 0x29,
        TAP_AXES = 0x2A,
        ACT_TAP_STATUS = 0x2B,
        BW_RATE = 0x2C,
        POWER_CTL = 0x2D,
        INT_ENABLE = 0x2E,
        INT_MAP = 0x2F,
        INT_SOURCE = 0x30,
        DATA_FORMAT = 0x31,
        DATAX0 = 0x32,
        DATAX1 = 0x33,
        DATAY0 = 0x34,
        DATAY1 = 0x35,
        DATAZ0 = 0x36,
        DATAZ1 = 0x37,
        FIFO_CTL = 0x38,
        FIFO_STATUS = 0x39,
    } RegisterName;
  
    uint8_t readReg(RegisterName reg);
    bool writeReg(RegisterName reg, uint8_t value);
    
    bool isValid() const {return m_valid;}
    bool readAcc();
    
    float accX() const {return m_value[0];}
    float accY() const {return m_value[1];}
    float accZ() const {return m_value[2];}
    
private:
    I2c *m_dev;
    bool m_valid;
    
public:
    int16_t m_acc[3];
    float m_value[3];
};

#endif