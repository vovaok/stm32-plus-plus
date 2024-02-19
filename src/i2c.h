#ifndef _I2C_H
#define _I2C_H

#include "gpio.h"
#include "rcc.h"

class I2c
{    
public:
    I2c(Gpio::Config pinSDA, Gpio::Config pinSCL);
    
    void setBusClock(int clk_Hz);
    void setAddress(uint8_t address);
    
    void open();
    void close();
    
    // high-level interface (some implementation)
    bool readReg(uint8_t hw_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size);
    bool writeReg(uint8_t hw_addr, uint8_t reg_addr, const uint8_t *buffer, uint8_t size);
    
    // common interface:
    bool read(uint8_t address, uint8_t *data, uint16_t size);
    bool write(uint8_t address, uint8_t *data, uint16_t size);
    
    // low-level interface:
    typedef enum
    {
        DirectionTransmitter    = 0x00,
        DirectionReceiver       = 0x01
    } Direction;
    
    bool startTransmission(Direction dir, uint8_t slaveAddress);
    bool stopTransmission();
    bool writeData(uint8_t data);
    bool readData(uint8_t *buf);
    void setAcknowledge(bool state);
    
private:
    I2C_TypeDef *m_dev;
    unsigned char lastAddress;
    
    typedef enum
    {
        EventMasterModeSelect                       = 0x00030001,
        EventMasterTransmitterModeSelected          = 0x00070082,
        EventMasterReceiverModeSelected             = 0x00030002,
        EventMasterModeAddress10                    = 0x00030008,
        EventMasterByteReceived                     = 0x00030040,
        EventMasterByteTransmitting                 = 0x00070080,
        EventMasterByteTransmitted                  = 0x00070084,
        EventSlaveReceiverAddressMatched            = 0x00020002,
        EventSlaveTransmitterAddressMatched         = 0x00060082,
        EventSlaveReceiverSecondAddressMatched      = 0x00820000,
        EventSlaveTransmitterSecondAddressMatched   = 0x00860080,
        EventSlaveGenerallCallAddressMatched        = 0x00120000,
        EventSlaveByteReceived                      = 0x00020040,
        EventSlaveStopDetected                      = 0x00000010,
        EventSlaveByteTransmitted                   = 0x00060084,
        EventSlaveByteTransmitting                  = 0x00060080,
        EventSlaveAckFailure                        = 0x00000400
    } Event;

    bool checkEvent(Event e);
};

#endif
