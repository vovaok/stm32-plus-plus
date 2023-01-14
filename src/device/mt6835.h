#ifndef _MT6835_H
#define _MT6835_H

#include "spi.h"
#include "absoluteencoder.h"

class MT6835 : public AbsoluteEncoder
{
public:
    MT6835(Spi *spi, Gpio::PinName csPin);
  
protected:
    virtual uint32_t readValue();
    
    typedef enum
    {
        USER_ID     = 0x001,
        ANGLE1      = 0x003,
        ANGLE2      = 0x004,
        ANGLE3      = 0x005,
        CRC_VALUE   = 0x006,
        ABZ_RES1    = 0x007,
        ABZ_RES2    = 0x008,
        ZERO_POS1   = 0x009,
        ZERO_POS2   = 0x00A,
        UVW_RES     = 0x00B,
        PWM_SEL     = 0x00C,
        ROT_DIR_HYST= 0x00D,
        AUTOCAL     = 0x00E,
        BW          = 0x011,
        NLC_Byte0   = 0x013, // NLC Byte 0x00~0xBF (Total 192 Bytes)
    } Register;
    
    void writeReg(Register reg, uint8_t value);
    uint8_t readReg(Register reg);
    
private:
    Spi *m_spi;
    Gpio *m_cs;
    
    typedef enum
    {
        CmdRead     = 0x30,
        CmdWrite    = 0x60,
        CmdProgram  = 0xC0,
        CmdSetZero  = 0x50,
        CmdBurstRead= 0xA0
    } Command;
    
    typedef enum
    {
        Overspeed = 0x01,
        WeakMagneticField = 0x02,
        Undervoltage = 0x04
    } StatusFlags;
};

#endif