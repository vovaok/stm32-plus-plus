#ifndef _DMA_L4_H
#define _DMA_L4_H

#include "stm32.h"
#include "core/core.h"

#define FOR_EACH_DMA(f) \
    f(1,1) f(1,2) f(1,3) f(1,4) f(1,5) f(1,6) f(1,7) \
    f(2,1) f(2,2) f(2,3) f(2,4) f(2,5) f(2,6) f(2,7)
#define DECLARE_DMA_IRQ_HANDLER(x,y) extern "C" void DMA##x##_Channel##y##_IRQHandler();
#define DECLARE_FRIEND(x,y) friend void DMA##x##_Channel##y##_IRQHandler();
//---------------------------------------------------------------------------

FOR_EACH_DMA(DECLARE_DMA_IRQ_HANDLER)
//---------------------------------------------------------------------------
      
class Dma
{
public:
    typedef enum
    {
        // DMA1
        ADC1_Channel1 = 0x110,
        ADC2_Channel2 = 0x120,
        ADC3_Channel3 = 0x130,
        DFSDM1_FLT0 = 0x140,
        DFSDM1_FLT1 = 0x150,
        DFSDM1_FLT2 = 0x160,
        DFSDM1_FLT3 = 0x170,
        SPI1_RX_Channel2 = 0x121,
        SPI1_TX_Channel3 = 0x131,
        SPI2_RX_Channel4 = 0x141,
        SPI2_TX_Channel5 = 0x151,
        SAI2_A_Channel6 = 0x161,
        SAI2_B_Channel7 = 0x171,
        USART3_TX_Channel2 = 0x122,
        USART3_RX_Channel3 = 0x132,
        USART1_TX_Channel4 = 0x142,
        USART1_RX_Channel5 = 0x152,
        USART2_RX_Channel6 = 0x162,
        USART2_TX_Channel7 = 0x172,
        I2C3_TX_Channel2 = 0x123,
        I2C3_RX_Channel3 = 0x133,
        I2C2_TX_Channel4 = 0x143,
        I2C2_RX_Channel5 = 0x153,
        I2C1_TX_Channel6 = 0x163,
        I2C1_RX_Channel7 = 0x173,
        TIM2_CH3_Channel1 = 0x114,
        TIM2_UP_Channel2 = 0x124,
        TIM16_CH1_Channel3 = 0x134,
        TIM16_UP_Channel3 = 0x134,
        TIM2_CH1_Channel5 = 0x154,
        TIM16_CH1_Channel6 = 0x164,
        TIM16_UP_Channel6 = 0x164,
        TIM2_CH2_Channel7 = 0x174,
        TIM2_CH4_Channel7 = 0x174,
        TIM17_CH1_Channel1 = 0x115,
        TIM17_UP_Channel1 = 0x115,
        TIM3_CH3_Channel2 = 0x125,
        TIM3_CH4_Channel3 = 0x135,
        TIM3_UP_Channel3 = 0x135,
        TIM7_UP_Channel4 = 0x145,
        DAC_CH2_Channel4 = 0x145,
        QUADSPI_Channel5 = 0x155,
        TIM3_CH1_Channel6 = 0x165,
        TIM3_TRIG_Channel6 = 0x165,
        TIM17_CH1_Channel7 = 0x175,
        TIM17_UP_Channel7 = 0x175,
        TIM4_CH1_Channel1 = 0x116,
        TIM6_UP_Channel3 = 0x136,
        DAC_CH1_Channel3 = 0x136,
        TIM4_CH2_Channel4 = 0x146,
        TIM4_CH3_Channel5 = 0x156,
        TIM4_UP_Channel7 = 0x176,
        TIM1_CH1_Channel2 = 0x127,
        TIM1_CH2_Channel3 = 0x137,
        TIM1_CH4_Channel4 = 0x147,
        TIM1_TRIG_Channel4 = 0x147,
        TIM1_COM_Channel4 = 0x147,
        TIM15_CH1_Channel5 = 0x157,
        TIM15_UP_Channel5 = 0x157,
        TIM15_TRIG_Channel5 = 0x157,
        TIM15_COM_Channel5 = 0x157,
        TIM1_UP_Channel6 = 0x167,
        TIM1_CH3_Channel7 = 0x177,
        
        // DMA2
        I2C4_RX_Channel1 = 0x210,
        I2C4_TX_Channel2 = 0x220,
        ADC1_Channel3 = 0x230,
        ADC2_CHannel4 = 0x240,
        ADC3_Channel5 = 0x250,
        DCMI_Channel6 = 0x260,
        SAI1_A_Channel1 = 0x211,
        SAI1_B_Channel2 = 0x221,
        SAI2_A_Channel3 = 0x231,
        SAI2_B_Channel4 = 0x241,
        SAI1_A_Channel6 = 0x261,
        SAI1_B_Channel7 = 0x271,
        UART5_TX_Channel1 = 0x212,
        UART5_RX_Channel2 = 0x222,
        UART4_TX_Channel3 = 0x232,
        UART4_RX_Channel5 = 0x252,
        USART1_TX_Channel6 = 0x262,
        USART1_RX_Channel7 = 0x272,
        SPI3_RX_Channel1 = 0x213,
        SPI3_TX_Channel2 = 0x223,
        TIM6_UP_Channel4 = 0x243,
        DAC_CH1_Channel4 = 0x243,
        TIM7_UP_Channel5 = 0x253,
        DAC_CH2_Channel5 = 0x253,
        QUADSPI_Channel7 = 0x273,
        SWPMI1_RX_Channel1 = 0x214,
        SWPMI1_TX_Channel2 = 0x224,
        SPI1_RX_Channel3 = 0x234,
        SPI1_TX_Channel4 = 0x244,
        DCMI_Channel5 = 0x254,
        LPUART1_TX_Channel6 = 0x264,
        LPUART1_RX_Channel7 = 0x274,
        TIM5_CH4_Channel1 = 0x215,
        TIM5_TRIG_Channel1 = 0x215,
        TIM5_CH3_Channel2 = 0x225,
        TIM5_UP_Channel2 = 0x225,
        TIM5_CH2_Channel4 = 0x245,
        TIM5_CH1_Channel5 = 0x255,
        I2C1_RX_Channel6 = 0x265,
        I2C1_TX_Channel7 = 0x275,
        AES_IN_Channel1 = 0x216,
        AES_OUT_Channel2 = 0x226,
        AES_OUT_Channel3 = 0x236,
        AES_IN_Channel5 = 0x256,
        HASH_IN_Channel7 = 0x276,
        TIM8_CH3_Channel1 = 0x217,
        TIM8_UP_Channel1 = 0x217,
        TIM8_CH4_Channel2 = 0x227,
        TIM8_TRIG_Channel2 = 0x227,
        TIM8_COM_Channel2 = 0x227,
        SDMMC1_Channel4 = 0x247,
        SDMMC1_Channel5 = 0x257,
        TIM8_CH1_Channel6 = 0x267,
        TIM8_CH2_Channel7 = 0x277        
    } Channel;
  
private:
    static Dma *mChannels[14];
    DMA_TypeDef *mDma;
    DMA_Channel_TypeDef *mChannel;
    uint8_t mChannelNum;
    uint8_t mChannelSel;
    
#pragma pack(push,1)
    union
    {
        uint32_t all;
        struct
        {
            uint32_t EN: 1;
            uint32_t TCIE: 1;
            uint32_t HTIE: 1;
            uint32_t TEIE: 1;
            uint32_t DIR: 1;
            uint32_t CIRC: 1;
            uint32_t PINC: 1;
            uint32_t MINC: 1;
            uint32_t PSIZE: 2;
            uint32_t MSIZE: 2;
            uint32_t PL: 2;
            uint32_t MEM2MEM: 1;
        };
    } mConfig;
#pragma pack(pop)
    
    IRQn_Type mIrq;
    NotifyEvent mOnTransferComplete;
    
    typedef enum
    {
        GIF     = 0x01, // global interrupt flag
        TCIF    = 0x02, // transfer complete interrupt flag
        HTIF    = 0x04, // half transfer interrupt flag
        TEIF    = 0x08, // transfer error interrupt flag        
        AllFlags= 0x0f
    } Flag;
    
    bool testFlag(uint32_t flag) const;
    void clearFlag(uint32_t flag);
    
//    void tryInit();
    
    FOR_EACH_DMA(DECLARE_FRIEND)
    
    void handleInterrupt();
    
    void setPeriph(void *periph, int dataSize, bool isSource);
  
public:
    Dma(Channel channelName);
    ~Dma();
    
    void setSingleBuffer(void *buffer, int size);
    void setCircularBuffer(void *buffer, int size);
    void setMemorySource(void *ptr, int dataSize);
    void setMemorySource(uint8_t *ptr);
    void setMemorySource(uint16_t *ptr);
    void setMemorySource(uint32_t *ptr);
//    void setDoubleBuffer(void *buffer, void *buffer2, int size); // not available in L4 family
    void setSource(void *periph, int dataSize);
    void setSink(void *periph, int dataSize);
    
    void start(int size=0);
    void stop(bool wait=false);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    bool isComplete();
    
//    int currentPage() const; // not available in L4 family
    inline int dataCounter() const {return mChannel->CNDTR;}
    
    void setTransferCompleteEvent(NotifyEvent event);
};

#endif
