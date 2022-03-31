#ifndef _I2C_H
#define _I2C_H
#include "gpio.h"


class I2c
{
private:
    int mAddress;
    int mClockSpeed;
    I2C_TypeDef *mI2c;
    unsigned char lastAddress;
    
public:
    I2c(int i2cNumber, int clockSpeed, int address, Gpio::Config pinSDA, Gpio::Config pinSCL);
    I2c(Gpio::Config pinSDA, Gpio::Config pinSCL, int clockSpeed=100000, unsigned char address=0);
    void init(void);
    bool startTransmission(uint8_t transmissionDirection, uint8_t slaveAddress);
    bool stopTransmission();
    bool writeData(uint8_t data);
    bool readData(unsigned char *buf);
    void setAcknowledge(bool state);
    
    bool regRead(unsigned char address, unsigned char index, unsigned char *buffer);
    bool multipleRead(unsigned char address, unsigned char index, void *buffer, unsigned char length);
    bool regWrite(unsigned char address, unsigned char index, unsigned char byte);
    bool multipleWrite(unsigned char address, unsigned char index, const void *buffer, unsigned char length);
    
    unsigned char failAddress;
};

#endif
