#ifndef _DMA4XX_H
#define _DMA4XX_H

#include "stm32.h"
#include "core/core.h"

#define FOR_EACH_DMA(f) \
    f(1,0) f(1,1) f(1,2) f(1,3) f(1,4) f(1,5) f(1,6) f(1,7) \
    f(2,0) f(2,1) f(2,2) f(2,3) f(2,4) f(2,5) f(2,6) f(2,7)
#define DECLARE_DMA_IRQ_HANDLER(x,y) extern "C" void DMA##x##_Stream##y##_IRQHandler();
#define DECLARE_FRIEND(x,y) friend void DMA##x##_Stream##y##_IRQHandler();
//---------------------------------------------------------------------------

FOR_EACH_DMA(DECLARE_DMA_IRQ_HANDLER)
//---------------------------------------------------------------------------

class Dma
{
public:
    typedef enum
    {
        // DMA1
        SPI3_RX_Stream0     = 0x100,
        I2C1_RX_Stream0     = 0x101,
        TIM4_CH1_Stream0    = 0x102,
        I2S3_EXT_RX_Stream0 = 0x103,
        UART5_RX_Stream0    = 0x104,
        UART8_TX_Stream0    = 0x105,
        TIM5_CH3_Stream0    = 0x106,
        TIM5_UP_Stream0     = 0x106,
        TIM2_UP_Stream1     = 0x113,
        TIM2_CH3_Stream1    = 0x113,
        USART3_RX_Stream1   = 0x114,
        UART7_TX_Stream1    = 0x115,
        TIM5_CH4_Stream1    = 0x116,
        TIM5_TRIG_Stream1   = 0x117,
        SPI3_RX_Stream2     = 0x120,
        TIM7_UP_Stream2     = 0x121,
        I2S3_EXT_RX_Stream2 = 0x122,
        I2C3_RX_Stream2     = 0x123,
        UART4_RX_Stream2    = 0x124,
        TIM3_CH4_Stream2    = 0x125,
        TIM3_UP_Stream2     = 0x125,
        TIM5_CH1_Stream2    = 0x126,
        I2C2_RX_Stream2     = 0x127,
        SPI2_RX_Stream3     = 0x130,
        TIM4_CH2_Stream3    = 0x132,
        I2S2_EXT_RX_Stream3 = 0x133,
        USART3_TX_Stream3   = 0x134,
        UART7_RX_Stream3    = 0x135,
        TIM5_CH4_Stream3    = 0x136,
        TIM5_TRIG_Stream3   = 0x136,
        I2C2_RX_Stream3     = 0x137,
        SPI2_TX_Stream4     = 0x140,
        TIM7_UP_Stream4     = 0x141,
        I2S2_EXT_TX_Stream4 = 0x142,
        I2C3_TX_Stream4     = 0x143,
        UART4_TX_Stream4    = 0x144,
        TIM3_CH1_Stream4    = 0x145,
        TIM3_TRIG_Stream4   = 0x145,
        TIM5_CH2_Stream4    = 0x146,
        USART3_TX_Stream4   = 0x147,
        SPI3_TX_Stream5     = 0x150,
        I2C1_RX_Stream5     = 0x151,
        I2S3_EXT_TX_Stream5 = 0x152,
        TIM2_CH1_Stream5    = 0x153,
        USART2_RX_Stream5   = 0x154,
        TIM3_CH2_Stream5    = 0x155,
        DAC1_Stream5        = 0x157,
        I2C1_TX_Stream6     = 0x161,
        TIM4_UP_Stream6     = 0x162,
        TIM2_CH2_Stream6    = 0x163,
        TIM2_CH4_Stream6    = 0x163,
        USART2_TX_Stream6   = 0x164,
        UART8_RX_Stream6    = 0x165,
        TIM5_UP_Stream6     = 0x166,
        DAC2_Stream6        = 0x167,
        SPI3_TX_Stream7     = 0x170,
        I2C1_TX_Stream7     = 0x171,
        TIM4_CH3_Stream7    = 0x172,
        TIM2_UP_Stream7     = 0x173,
        TIM2_CH4_Stream7    = 0x173,
        UART5_TX_Stream7    = 0x174,
        TIM3_CH3_Stream7    = 0x175,
        I2C2_TX_Stream7     = 0x177,

        // DMA2
        ADC1_Stream0        = 0x200,
        ADC3_Stream0        = 0x202,
        SPI1_RX_Stream0     = 0x203,
        SPI4_RX_Stream0     = 0x204,
        TIM1_TRIG_Stream0   = 0x206,
        SAI1_A_Stream1      = 0x210,
        DCMI_Stream1        = 0x211,
        ADC3_Stream1        = 0x212,
        SPI4_TX_Stream1     = 0x214,
        USART6_RX_Stream1   = 0x215,
        TIM1_CH1_Stream1    = 0x216,
        TIM8_UP_Stream1     = 0x217,
        TIM8_CH1_Stream2    = 0x220,
        TIM8_CH2_Stream2    = 0x220,
        TIM8_CH3_Stream2    = 0x220,
        ADC2_Stream2        = 0x221,
        SPI1_RX_Stream2     = 0x223,
        USART1_RX_Stream2   = 0x224,
        USART6_RX_Stream2   = 0x225,
        TIM1_CH2_Stream2    = 0x226,
        TIM8_CH1_Stream2_Ch7= 0x227,
        SAI1_A_Stream3      = 0x230,
        ADC2_Stream3        = 0x231,
        SPI5_RX_Stream3     = 0x232,
        SPI1_TX_Stream3     = 0x233,
        SDIO_Stream3        = 0x234,
        SPI4_RX_Stream3     = 0x235,
        TIM1_CH1_Stream3    = 0x236,
        TIM8_CH2_Stream3    = 0x237,
        ADC1_Stream4        = 0x240,
        SAI1_B_Stream4      = 0x241,
        SPI5_TX_Stream4     = 0x242,
        SPI4_TX_Stream4     = 0x245,
        TIM1_CH4_Stream4    = 0x246,
        TIM1_TRIG_Stream4   = 0x246,
        TIM1_COM_Stream4    = 0x246,
        TIM8_CH3_Stream4    = 0x247,
        SAI1_B_Stream5      = 0x250,
        SPI6_TX_Stream5     = 0x251,
        CRYP_OUT_Stream5    = 0x252,
        SPI1_TX_Stream5     = 0x253,
        USART1_RX_Stream5   = 0x254,
        TIM1_UP_Stream5     = 0x256,
        SPI5_RX_Stream5     = 0x257,
        TIM1_CH1_Stream6    = 0x260,
        TIM1_CH2_Stream6    = 0x260,
        TIM1_CH3_Stream6    = 0x260,
        SPI6_RX_Stream6     = 0x261,
        CRYP_IN_Stream6     = 0x262,
        SDIO_Stream6        = 0x264,
        USART6_TX_Stream6   = 0x265,
        TIM1_CH3_Stream6_Ch6= 0x266,
        SPI5_TX_Stream6     = 0x267,
        DCMI_Stream7        = 0x271,
        HASH_IN_Stream7     = 0x272,
        USART1_TX_Stream7   = 0x274,
        USART6_TX_Stream7   = 0x275,
        TIM8_CH4_Stream7    = 0x277,
        TIM8_TRIG_Stream7   = 0x277,
        TIM8_COM_Stream7    = 0x277
    } Channel;

private:
    static Dma *mStreams[16];
    DMA_TypeDef *mDma;
    DMA_Stream_TypeDef *mStream;
//    uint8_t mStreamNum; // not used now, replaced with flags offset
//    uint8_t mChannelNum; // commented because of it is given in the mConfig.CHSEL
    volatile uint32_t *mISR = nullptr; // LISR or HISR depending on stream number
    volatile uint32_t *mIFCR = nullptr; // LIFCR or HIFCR depending on stream number
    int mFlagsOffset = 0;

#pragma pack(push,1)
    union
    {
        uint32_t all;
        struct
        {
            uint32_t EN: 1;
            uint32_t DMEIE: 1;
            uint32_t TEIE: 1;
            uint32_t HTIE: 1;
            uint32_t TCIE: 1;
            uint32_t PFCTRL: 1;
            uint32_t DIR: 2;
            uint32_t CIRC: 1;
            uint32_t PINC: 1;
            uint32_t MINC: 1;
            uint32_t PSIZE: 2;
            uint32_t MSIZE: 2;
            uint32_t PINCOS: 1;
            uint32_t PL: 2;
            uint32_t DBM: 1;
            uint32_t CT: 1;
            uint32_t : 1;
            uint32_t PBURST: 2;
            uint32_t MBURST: 2;
            uint32_t CHSEL: 3;
        };
    } mConfig;
#pragma pack(pop)

    IRQn_Type mIrq;
    NotifyEvent mOnTransferComplete;

    typedef enum
    {
        FEIF    = 0x01, // FIFO error interrupt flag
        DMEIF   = 0x04, // direct mode error interrupt flag
        TEIF    = 0x08, // transfer error interrupt flag
        HTIF    = 0x10, // half transfer interrupt flag
        TCIF    = 0x20, // transfer complete interrupt flag
        AllFlags= 0x3d
    } Flag;

    bool testFlag(uint32_t flag) const;
    void clearFlag(uint32_t flag);

//    void tryInit();

    FOR_EACH_DMA(DECLARE_FRIEND)

    void handleInterrupt();

    void setPeriph(volatile void *periph, int dataSize, bool isSource);

public:
    Dma(Channel channelName);
    ~Dma();
    
    //! Get DMA instance
    //! Use it to obtain the instance of the DMA stream for desired channel
    //! in case of several periperals share the same stream
    static Dma *instance(Channel channelName);

    void setSingleBuffer(void *buffer, int size);
    void setCircularBuffer(void *buffer, int size);
    void setDoubleBuffer(void *buffer, void *buffer2, int size);
    void setMemorySource(void *ptr, int dataSize);
    void setMemorySource(uint8_t *ptr);
    void setMemorySource(uint16_t *ptr);
    void setMemorySource(uint32_t *ptr);
    void setSource(volatile void *periph, int dataSize);
    void setSink(volatile void *periph, int dataSize);

    inline void start() {mStream->CR |= DMA_SxCR_EN;}
    void start(int size);
    void stop(bool wait=false);
    void setEnabled(bool enabled);
    bool isEnabled() const;

    bool isComplete();

    int currentPage() const;
    inline int dataCounter() const {return mStream->NDTR;}

    void setTransferCompleteEvent(NotifyEvent event);
};

#endif
