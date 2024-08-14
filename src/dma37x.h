#ifndef _DMA37X_H
#define _DMA37X_H

#include "stm32.h"
#include "core/coreexception.h"
#include "core/coretypes.h"

typedef struct
{
  __IO uint32_t CCR;          //!< DMA channel x configuration register
  __IO uint32_t CNDTR;        //!< DMA channel x number of data register 
  __IO uint32_t CPAR;         //!< DMA channel x peripheral address register
  __IO uint32_t CMAR;         //!< DMA channel x memory address register 
  __IO uint32_t _RESERVED;
} DMA_Channel;

typedef struct
{
    DMA_TypeDef status;
    DMA_Channel channel[7];
} DmaPeriph;

#if defined(DMA1)
#undef DMA1
#define DMA1 ((DmaPeriph*)DMA1_BASE)
#endif

#if defined(DMA2)
#undef DMA2
#define DMA2 ((DmaPeriph*)DMA2_BASE)
#endif
//--------------------------------------------------------------------------

#define FOR_EACH_DMA(f) \
    f(1,1) f(1,2) f(1,3) f(1,4) f(1,5) f(1,6) f(1,7)\
    f(2,1) f(2,2) f(2,3) f(2,4) f(2,5)
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
        Channel1_ADC1       = 0x10,
        Channel1_TIM2_CH3   = 0x10,
        Channel1_TIM4_CH1   = 0x10,
        Channel1_TIM17_CH1  = 0x10,
        Channel1_TIM17_UP   = 0x10,
        Channel1_TIM19_CH3  = 0x10,
        Channel1_TIM19_CH4  = 0x10,
        Channel2_SPI1_RX    = 0x11,
        Channel2_USART3_TX  = 0x11,
        Channel2_TIM2_UP    = 0x11,
        Channel2_TIM3_CH3   = 0x11,
        Channel2_TIM19_CH1  = 0x11,
        Channel3_SPI1_TX    = 0x12,
        Channel3_USART3_RX  = 0x12,
        Channel3_TIM3_CH4   = 0x12,
        Channel3_TIM3_UP    = 0x12,
        Channel3_TIM6_UP    = 0x12,
        Channel3_DAC1_CH1   = 0x12,
        Channel3_TIM16_CH1  = 0x12,
        Channel3_TIM16_UP   = 0x12,
        Channel3_TIM19_CH2  = 0x12,
        Channel4_SPI2_RX    = 0x13,
        Channel4_USART1_TX  = 0x13,
        Channel4_I2C2_TX    = 0x13,
        Channel4_TIM4_CH2   = 0x13,
        Channel4_TIM7_UP    = 0x13,
        Channel4_DAC1_CH2   = 0x13,
        Channel4_TIM19_UP   = 0x13,
        Channel5_SPI2_TX    = 0x14,
        Channel5_USART1_RX  = 0x14,
        Channel5_I2C2_RX    = 0x14,
        Channel5_TIM2_CH1   = 0x14,
        Channel5_TIM4_CH3   = 0x14,
        Channel5_TIM18_UP   = 0x14,
        Channel5_DAC2_CH1   = 0x14,
        Channel5_TIM15_CH1  = 0x14,
        Channel5_TIM15_UP   = 0x14,
        Channel5_TIM15_TRIG = 0x14,
        Channel5_TIM15_COM  = 0x14,
        Channel6_USART2_RX  = 0x15,
        Channel6_I2C1_TX    = 0x15,
        Channel6_TIM3_CH1   = 0x15,
        Channel6_TIM3_TRIG  = 0x15,
        Channel6_TIM16_CH1  = 0x15,
        Channel6_TIM16_UP   = 0x15,
        Channel7_USART2_TX  = 0x16,
        Channel7_I2C1_RX    = 0x16,
        Channel7_TIM2_CH2   = 0x16,
        Channel7_TIM2_CH4   = 0x16,
        Channel7_TIM4_UP    = 0x16,
        Channel7_TIM17_CH1  = 0x16,
        Channel7_TIM17_UP   = 0x16,
        Channel1_SPI3_RX    = 0x20,
        Channel1_TIM5_CH4   = 0x20,
        Channel1_TIM5_TRIG  = 0x20,
        Channel2_SPI3_TX    = 0x21,
        Channel2_TIM5_CH3   = 0x21,
        Channel2_TIM5_UP    = 0x21,
        Channel3_SDADC1     = 0x22,
        Channel3_TIM6_UP_DMA2   = 0x22,
        Channel3_DAC1_CH1_DMA2  = 0x22,
        Channel4_SDADC2     = 0x23,
        Channel4_TIM5_CH2   = 0x23,
        Channel4_TIM7_UP_DMA2   = 0x23,
        Channel4_DAC1_CH2_DMA2  = 0x23,
        Channel5_SDADC3     = 0x24,
        Channel5_TIM5_CH1   = 0x24,
        Channel5_TIM18_UP_DMA2  = 0x24,
        Channel5_DAC2_CH1_DMA2  = 0x24,
    } DmaChannel;
  
private:
    static Dma *mDma1Channels[7];
    static Dma *mDma2Channels[5];
    DmaPeriph *mDma;
    DMA_Channel *mDmaChan;
    unsigned char mChannel;
    
    IRQn mIrq;
    NotifyEvent mOnTransferComplete;
    
    FOR_EACH_DMA(DECLARE_FRIEND)
      
    void handleInterrupt();
    
public:
    Dma(DmaChannel channelName);
    ~Dma();
    
    void setSingleBuffer(void *buffer, int size);
    void setCircularBuffer(void *buffer, int size);
    void setSource(void *periph, int dataSize);
    void setSink(void *periph, int dataSize);
    
    void start(int size=0);
    void stop(bool wait=false);
    void setEnabled(bool enable);
    inline bool isEnabled() const {return mDmaChan->CCR & DMA_CCR_EN;}
    
    inline int dataCounter() const {return mDmaChan->CNDTR;}
    
    void setTransferCompleteEvent(NotifyEvent event);    
};

#endif