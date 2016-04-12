#ifndef _I2C_H
#define _I2C_H
#include "gpio.h"


class I2c
{
private:
   int mAddress;
   int mClockSpeed;
   I2C_TypeDef *mI2c;
    
  
public:
  
  
  I2c(int i2cNumber, int clockSpeed, int address, Gpio::Config pinSDA, Gpio::Config pinSCL);
  void init(void);
  void startTransmission(uint8_t transmissionDirection,  uint8_t slaveAddress);
  void stopTransmission ();
  void writeData(uint8_t data);
  uint8_t readData();
    
};

#endif
