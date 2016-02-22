#ifndef _I2S_H
#define _I2S_H

#include "gpio.h"
#include "dma.h"

class I2S
{
private:
    SPI_TypeDef *mSpi;
    Dma *mRxDma, *mTxDma;
    bool mActive;
  
    static SPI_TypeDef *getSpiByPin(Gpio::Config pinConfig);
    
public:
    I2S(Gpio::Config clkPin, Gpio::Config doPin);
    
    void setActive(bool enable);
    bool isActive() const {return mActive;}
    void open() {setActive(true);}
    void close() {setActive(false);}
    
    Dma *dmaForRx();
    Dma *dmaForTx();
    void deleteDmaForRx();
    void deleteDmaForTx();
    
    void write(unsigned short data);
    unsigned short read();
};

#endif