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
    
    bool read(uint8_t address, uint8_t *data, uint16_t size);
    bool write(uint8_t address, uint8_t *data, uint16_t size);
    
//    bool regRead(unsigned char address, unsigned char index, unsigned char *buffer);
//    bool multipleRead(unsigned char address, unsigned char index, void *buffer, unsigned char length);
//    bool regWrite(unsigned char address, unsigned char index, unsigned char byte);
//    bool multipleWrite(unsigned char address, unsigned char index, const void *buffer, unsigned char length);
//    
//    unsigned char failAddress;
    
private:
    I2C_TypeDef *m_dev;
    unsigned char lastAddress;
    
    typedef enum
    {
        DirectionTransmitter    = 0x00,
        DirectionReceiver       = 0x01
    } Direction;
    
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
    bool startTransmission(Direction dir, uint8_t slaveAddress);
    bool stopTransmission();
    bool writeData(uint8_t data);
    bool readData(unsigned char *buf);
    void setAcknowledge(bool state);
};

#endif
