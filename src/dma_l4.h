#ifndef _DMA_L4_H
#define _DMA_L4_H

#include "stm32.h"
#include "core/core.h"

#if defined(STM32L4)
    #define DMA_CHANNEL_COUNT   14
    #define FOR_EACH_DMA(f) \
        f(1,1) f(1,2) f(1,3) f(1,4) f(1,5) f(1,6) f(1,7) \
        f(2,1) f(2,2) f(2,3) f(2,4) f(2,5) f(2,6) f(2,7)

#elif defined(STM32G431xx) | defined(STM32G441xx) // Category 2 devices
    #define DMA_CHANNEL_COUNT   12
    #define FOR_EACH_DMA(f) \
        f(1,1) f(1,2) f(1,3) f(1,4) f(1,5) f(1,6) \
        f(2,1) f(2,2) f(2,3) f(2,4) f(2,5) f(2,6)
            
#elif defined(STM32G4) // Category 3 and Category 4 devices
    #define DMA_CHANNEL_COUNT   16
    #define FOR_EACH_DMA(f) \
        f(1,1) f(1,2) f(1,3) f(1,4) f(1,5) f(1,6) f(1,7) f(1,8) \
        f(2,1) f(2,2) f(2,3) f(2,4) f(2,5) f(2,6) f(2,7) f(2,8)
            
#endif

#define DECLARE_DMA_IRQ_HANDLER(x,y) extern "C" void DMA##x##_Channel##y##_IRQHandler();
#define DECLARE_FRIEND(x,y) friend void DMA##x##_Channel##y##_IRQHandler();
//---------------------------------------------------------------------------

FOR_EACH_DMA(DECLARE_DMA_IRQ_HANDLER)
//---------------------------------------------------------------------------
      
class Dma
{
public:    
    enum Channel; // forward declaration
  
     Dma(Channel channelName);
    ~Dma();
    
    void setSingleBuffer(void *buffer, int size);
    void setCircularBuffer(void *buffer, int size);
    void setMemorySource(void *ptr, int dataSize);
    void setMemorySource(uint8_t *ptr);
    void setMemorySource(uint16_t *ptr);
    void setMemorySource(uint32_t *ptr);
//    void setDoubleBuffer(void *buffer, void *buffer2, int size); // not available in L4 and G4 families
    void setSource(volatile void *periph, int dataSize);
    void setSink(volatile void *periph, int dataSize);
    
    void start(int size=0);
    void stop(bool wait=false);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    bool isComplete();
    
//    int currentPage() const; // not available in L4 and G4 families
    inline int dataCounter() const {return mChannel->CNDTR;}
    
    void setTransferCompleteEvent(NotifyEvent event);
    
private:
    static Dma *mChannels[DMA_CHANNEL_COUNT];
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
    
    FOR_EACH_DMA(DECLARE_FRIEND)
    
    void handleInterrupt();
    
    void setPeriph(volatile void *periph, int dataSize, bool isSource);
};

#if defined(STM32L4)

enum Dma::Channel
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
};

#elif defined(STM32G4)

// this is resource ID used in the DMAMUX peripheral
enum Dma::Channel
{
    DMAMUX_Req_G0 = 1, 
	DMAMUX_Req_G1 = 2, 
	DMAMUX_Req_G2 = 3, 
	DMAMUX_Req_G3 = 4, 
	ADC1_ = 5, 
	DAC1_CH1 = 6, 
	DAC1_CH2 = 7, 
	TIM6_UP = 8, 
	TIM7_UP = 9, 
	SPI1_RX = 10, 
	SPI1_TX = 11, 
	SPI2_RX = 12, 
	SPI2_TX = 13, 
	SPI3_RX = 14, 
	SPI3_TX = 15, 
	I2C1_RX = 16, 
	I2C1_TX = 17, 
	I2C2_RX = 18, 
	I2C2_TX = 19, 
	I2C3_RX = 20, 
	I2C3_TX = 21, 
	I2C4_RX = 22, 
	I2C4_TX = 23, 
	USART1_RX = 24, 
	USART1_TX = 25, 
	USART2_RX = 26, 
	USART2_TX = 27, 
	USART3_RX = 28, 
	USART3_TX = 29, 
	UART4_RX = 30, 
	UART4_TX = 31, 
	UART5_RX = 32, 
	UART5_TX = 33, 
	LPUART1_RX = 34, 
	LPUART1_TX = 35, 
	ADC2_ = 36, 
	ADC3_ = 37, 
	ADC4_ = 38, 
	ADC5_ = 39, 
	QUADSPI_ = 40, 
	DAC2_CH1 = 41, 
	TIM1_CH1 = 42, 
	TIM1_CH2 = 43, 
	TIM1_CH3 = 44, 
	TIM1_CH4 = 45, 
	TIM1_UP = 46, 
	TIM1_TRIG = 47, 
	TIM1_COM = 48, 
	TIM8_CH1 = 49, 
	TIM8_CH2 = 50, 
	TIM8_CH3 = 51, 
	TIM8_CH4 = 52, 
	TIM8_UP = 53, 
	TIM8_TRIG = 54, 
	TIM8_COM = 55, 
	TIM2_CH1 = 56, 
	TIM2_CH2 = 57, 
	TIM2_CH3 = 58, 
	TIM2_CH4 = 59, 
	TIM2_UP = 60, 
	TIM3_CH1 = 61, 
	TIM3_CH2 = 62, 
	TIM3_CH3 = 63, 
	TIM3_CH4 = 64, 
	TIM3_UP = 65, 
	TIM3_TRIG = 66, 
	TIM4_CH1 = 67, 
	TIM4_CH2 = 68, 
	TIM4_CH3 = 69, 
	TIM4_CH4 = 70, 
	TIM4_UP = 71, 
	TIM5_CH1 = 72, 
	TIM5_CH2 = 73, 
	TIM5_CH3 = 74, 
	TIM5_CH4 = 75, 
	TIM5_UP = 76, 
	TIM5_TRIG = 77, 
	TIM15_CH1 = 78, 
	TIM15_UP = 79, 
	TIM15_TRIG = 80, 
	TIM15_COM = 81, 
	TIM16_CH1 = 82, 
	TIM16_UP = 83, 
	TIM17_CH1 = 84, 
	TIM17_UP = 85, 
	TIM20_CH1 = 86, 
	TIM20_CH2 = 87,
	TIM20_CH3 = 88,
	TIM20_CH4 = 89,
	TIM20_UP = 90,
	AES_IN = 91,
	AES_OUT = 92,
	TIM20_TRIG = 93,
	TIM20_COM = 94,
	HRTIM_MASTER = 95,
	HRTIM_TIMA = 96, 
	HRTIM_TIMB = 97, 
	HRTIM_TIMC = 98, 
	HRTIM_TIMD = 99, 
	HRTIM_TIME = 100, 
	HRTIM_TIMF = 101, 
	DAC3_CH1 = 102,
	DAC3_CH2 = 103,
	DAC4_CH1 = 104,
	DAC4_CH2 = 105,
	SPI4_RX = 106,
	SPI4_TX = 107,
	SAI1_A = 108,
	SAI1_B = 109,
	FMAC_Read = 110,
	FMAC_Write = 111,
	Cordic_Read = 112,
	Cordic_Write = 113,
	UCPD1_RX = 114,
	UCPD1_TX = 115
};

#endif

#endif
