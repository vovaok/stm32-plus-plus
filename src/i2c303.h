#ifndef _I2C303_H
#define _I2C303_H

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
    bool writeReg(uint8_t hw_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size);
    
    // common interface:
    bool read(uint8_t address, uint8_t *data, uint16_t size);
    bool write(uint8_t address, uint8_t *data, uint16_t size);    
    
   
    bool writeData(uint8_t data);
    bool readData(uint8_t *buf);  
    
private:
    I2C_TypeDef *m_dev;
    unsigned char lastAddress;    
   
};

#endif
