#ifndef _SPI_H
#define _SPI_H

#include "gpio.h"
#include "dma.h"

typedef enum
{
    SpiNone = 0,
    Spi1    = 1,
    Spi2    = 2,
    Spi3    = 3
} SpiNo;

typedef Closure<void(unsigned short)> SpiDataEvent;
//---------------------------------------------------------------------------

extern "C" void SPI1_IRQHandler(void);
extern "C" void SPI2_IRQHandler(void);
extern "C" void SPI3_IRQHandler(void);
//---------------------------------------------------------------------------

class Spi
{
public:
#pragma pack(push,1)
    union Config
    {
        unsigned short word;
        struct
        {
            unsigned char CPHA: 1;
            unsigned char CPOL: 1;
            unsigned char master: 1;
            unsigned char baudrate: 3;
            unsigned char enable: 1;
            unsigned char LSBfirst: 1;
            unsigned char SSI: 1;
            unsigned char SSM: 1;
            unsigned char RXonly: 1;
            unsigned char frame16bit: 1;
            unsigned char : 4; // internal use
        };
        Config() : word(0){}
    };
#pragma pack(pop)
  
private:  
    static Spi *mSpies[3];
    SPI_TypeDef *mDev;
    Config mConfig;
    IRQn mIrq;
    SpiDataEvent onTransferComplete;
    Dma::DmaChannel mDmaChannelRx;
    Dma::DmaChannel mDmaChannelTx;
    Dma *mDmaRx;
    Dma *mDmaTx;
    bool mUseDmaRx, mUseDmaTx;
    
    friend void SPI1_IRQHandler(void);
    friend void SPI2_IRQHandler(void);
    friend void SPI3_IRQHandler(void);
   
    void enableInterrupt();
    void handleInterrupt();
    
    SpiNo getSpiByPin(Gpio::Config pin);
  
public:
    explicit Spi(Gpio::Config sck, Gpio::Config miso, Gpio::Config mosi);
    
    void setConfig(Config cfg);
    
    void setUseDmaRx(bool useDma);
    void setUseDmaTx(bool useDma);
    
    void open();
    void close();
    inline bool isOpen() {return mConfig.enable;}
    
    unsigned short transferWord(unsigned short word=0xFFFF);
    void transferWordAsync(unsigned short word=0xFFFF);

    void setTransferCompleteEvent(SpiDataEvent e); 
    void transfer(unsigned char*,unsigned char);
};

#endif