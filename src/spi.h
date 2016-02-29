#ifndef _SPI_H
#define _SPI_H

#include "gpio.h"
#include "dma.h"
#define PI (3.141592653589793)


extern "C"  void SPI2_IRQHandler(void);
class Spi
{
  
  public:
    
    explicit Spi( SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct);  
    float* spiRead();
   
   

private:
 Gpio *cs[24];
 float u32result[24];
 int countCs;
 float zero[24];
 unsigned short temp[24];
 
 friend void SPI2_IRQHandler(void);
 static Spi *mSpi;
  void handleInterrupt();
  
 
 
};

#endif