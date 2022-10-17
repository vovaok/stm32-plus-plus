#ifndef _SPI_H
#define _SPI_H

#include "core/core.h"
#include "gpio.h"
//#include "dma.h"

typedef Closure<void(unsigned short)> SpiDataEvent;
//---------------------------------------------------------------------------

extern "C" void SPI1_IRQHandler(void);
extern "C" void SPI2_IRQHandler(void);
extern "C" void SPI3_IRQHandler(void);
extern "C" void SPI4_IRQHandler(void);
extern "C" void SPI5_IRQHandler(void);
extern "C" void SPI6_IRQHandler(void);
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
    static Spi *mSpies[6];
    SPI_TypeDef *mDev;
    Config mConfig;
    IRQn_Type mIrq;
    SpiDataEvent onTransferComplete;
    
//    Dma::DmaChannel mDmaChannelRx;
//    Dma::DmaChannel mDmaChannelTx;
//    Dma *mDmaRx;
//    Dma *mDmaTx;
//    bool mUseDmaRx, mUseDmaTx;
    
    friend void SPI1_IRQHandler(void);
    friend void SPI2_IRQHandler(void);
    friend void SPI3_IRQHandler(void);
    friend void SPI4_IRQHandler(void);
    friend void SPI5_IRQHandler(void);
    friend void SPI6_IRQHandler(void);
   
    void enableInterrupt();
    void handleInterrupt();
  
public:
    explicit Spi(Gpio::Config sck, Gpio::Config miso, Gpio::Config mosi);
    
    void setConfig(Config cfg);
    
    void setMasterMode();
    void setCPOL_CPHA(bool CPOL, bool CPHA);
    void setBaudratePrescaler(int psc);
    
//    void setUseDmaRx(bool useDma);
//    void setUseDmaTx(bool useDma);
    
    void open();
    void close();
    inline bool isOpen() {return mConfig.enable;}
    
//    uint16_t transferWord(uint16_t word=0xFFFF);
    void transferWordAsync(uint16_t word=0xFFFF);

    void setTransferCompleteEvent(SpiDataEvent e); 
    void transfer(uint8_t* data, int size);
    void transfer(const uint8_t *data, uint8_t *buffer, int size);
    
    uint16_t read();
    uint16_t write(uint16_t word);
    
    void read(uint8_t* data, int size);
    void write(const uint8_t *data, int size);
};

#endif