#ifndef _DRV8320_H
#define _DRV8320_H

#include "spi.h"

class Drv8320
{
public:
    Drv8320(Spi *spi, Gpio::PinName csPin, Gpio::PinName enablePin);
    
    void setFaultPin(Gpio::PinName pin);
    
    bool isFault() const;
    
private:
    Spi *m_spi;
    Gpio *m_csPin;
    Gpio *m_enablePin;
    Gpio *m_faultPin;
    
    enum Register
    {
        FaultStatus1    = 0,
        FaultStatus2    = 1,
        DriverControl   = 2,
        GateDriveHS     = 3,
        GateDriveLS     = 4,
        OcpControl      = 5
    };

    void writeReg(uint8_t reg, uint16_t data);
    uint16_t readReg(uint8_t reg);
};

#endif