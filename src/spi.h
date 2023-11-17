#ifndef _SPI_H
#define _SPI_H

#include "core/core.h"
#include "gpio.h"
#include "dma.h"

typedef Closure<void(unsigned short)> SpiDataEvent;
//---------------------------------------------------------------------------

extern "C" void SPI1_IRQHandler(void);
extern "C" void SPI2_IRQHandler(void);
extern "C" void SPI3_IRQHandler(void);
extern "C" void SPI4_IRQHandler(void);
extern "C" void SPI5_IRQHandler(void);
extern "C" void SPI6_IRQHandler(void);
//---------------------------------------------------------------------------

#if defined(STM32F4)
    #define SPI_CR1_DFF_BIT     DFF     // data frame format (8/16 bit)
    #define SPI_CR2_NSSP_BIT
#elif defined(STM32L4) || defined(STM32G4)
    #define SPI_FIFO_IMPL   1
    #define SPI_CR1_DFF_BIT     CRCL    // CRC length
    #define SPI_CR2_NSSP_BIT    NSSP
#endif

class Spi
{
public:
#pragma pack(push,1)
    union Config
    {
        struct
        {
            uint16_t cr1;
            uint16_t cr2;
        };

        struct
        {
            uint8_t CPHA: 1;
            uint8_t CPOL: 1;
            uint8_t master: 1;
            uint8_t baudrate: 3;
            uint8_t enable: 1;
            uint8_t LSBfirst: 1;
            uint8_t SSI: 1;
            uint8_t SSM: 1;
            uint8_t RXonly: 1;
            uint8_t SPI_CR1_DFF_BIT: 1;
            uint8_t crcNext: 1; // transmit CRC instead of data
            uint8_t crcEnable: 1; // CRC hardware calculation enable
            uint8_t BIDIOE: 1;
            uint8_t BIDIMODE: 1;

            uint8_t RXDMAEN: 1;
            uint8_t TXDMAEN: 1;
            uint8_t SSOE: 1;
            uint8_t SPI_CR2_NSSP_BIT: 1;
            uint8_t FRF: 1;
            uint8_t ERRIE: 1;
            uint8_t RXNEIE: 1;
            uint8_t TXEIE: 1;
#if defined(SPI_FIFO_IMPL) && SPI_FIFO_IMPL == 1
            uint8_t DS: 4;
            uint8_t FRXTH: 1;
            uint8_t LDMA_RX: 1;
            uint8_t LDMA_TX: 1;
#endif
        };
        Config() : cr1(0x0000), cr2(0x0700) {}
    };
#pragma pack(pop)

private:
    static Spi *mSpies[6];
    SPI_TypeDef *mDev;
    Config mConfig;
    IRQn_Type mIrq;
    SpiDataEvent onTransferComplete;

    Dma::Channel mDmaChannelRx;
    Dma::Channel mDmaChannelTx;
    Dma *mDmaRx;
    Dma *mDmaTx;
    bool mUseDmaRx, mUseDmaTx;
    uint8_t m_dataSize;

    friend void SPI1_IRQHandler(void);
    friend void SPI2_IRQHandler(void);
    friend void SPI3_IRQHandler(void);
    friend void SPI4_IRQHandler(void);
    friend void SPI5_IRQHandler(void);
    friend void SPI6_IRQHandler(void);

    void enableInterrupt();
    void handleInterrupt();
    void handleDmaInterrupt();

    void updateConfig();

public:
    explicit Spi(Gpio::Config sck, Gpio::Config miso, Gpio::Config mosi);

    void setConfig(Config cfg);

    void setMasterMode();
    void setCPOL_CPHA(bool CPOL, bool CPHA);
    void setBaudratePrescaler(int psc);

    void setUseDmaRx(bool useDma);
    void setUseDmaTx(bool useDma);

    void open();
    void close();
    inline bool isOpen() {return mConfig.enable;}

//    uint16_t transferWord(uint16_t word=0xFFFF);
    void transferWordAsync(uint16_t word=0xFFFF);

    void setTransferCompleteEvent(SpiDataEvent e);
    void transfer(uint8_t* data, int size);
    void transfer(const uint8_t *data, uint8_t *buffer, int size);
    void setDataSize(int size);

    Dma *dmaTx() {return mDmaTx;}

    uint8_t read();
    uint8_t write(uint8_t word);
    uint16_t read16();
    uint16_t write16(uint16_t word);

    void read(uint8_t* data, int size);
    bool write(const uint8_t *data, int size);

    // write (count) words of the same values (*value)
    bool writeFill16(uint16_t *value, int count);
    // write cyclic pattern
    bool writeFill16(uint16_t *pattern, int patternSize, int count);

    void waitForBytesWritten();

    NotifyEvent onBytesWritten;
};

#endif