#ifndef _DMA303X_H
#define _DMA303X_H

#include "stm32.h"
#include "core/coreexception.h"
#include "core/coretypes.h"
#include "stm32.h"
#include "core/core.h"


    #define DMA_CHANNEL_COUNT   7
    #define FOR_EACH_DMA(f) \
        f(1,1) f(1,2) f(1,3) f(1,4) f(1,5) f(1,6) f(1,7) 
            


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
    static Dma *instance(Channel channelName);

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


  enum Dma::Channel
    {
        Channel1_ADC1       = 0x10,
        Channel1_TIM2_CH3   = 0x10,       
        Channel1_TIM17_CH1  = 0x10,
        Channel1_TIM17_UP   = 0x10,
      
        Channel2_SPI1_RX    = 0x11,
        Channel2_USART3_TX  = 0x11,
        Channel2_TIM1_CH1   = 0x11,
        Channel2_TIM2_UP    = 0x11,
        Channel2_TIM3_CH3   = 0x11,
        Channel2_ADC2       = 0x11,
        Channel2_I2C1_TX    = 0x11,
        
        Channel3_SPI1_TX    = 0x12,
        Channel3_USART3_RX  = 0x12,
        Channel3_TIM1_CH2   = 0x12,        
        Channel3_TIM6_UP    = 0x12,
        Channel3_DAC1_CH1   = 0x12,
        Channel3_TIM16_CH1  = 0x12,
        Channel3_TIM16_UP   = 0x12,
        Channel3_I2C1_RX    = 0x12,        
       
        Channel4_SPI1_RX    = 0x13,
        Channel4_USART1_TX  = 0x13,
        Channel4_I2C1_TX    = 0x13,
        Channel4_TIM1_CH4   = 0x13,
        Channel4_TIM1_TRIG  = 0x13,
        Channel4_TIM1_COM   = 0x13,
        Channel4_TIM7_UP    = 0x13,
        Channel4_DAC1_CH2   = 0x13,
        Channel4_ADC2       = 0x13,
        
        Channel5_SPI1_TX    = 0x14,
        Channel5_USART1_RX  = 0x14,
        Channel5_I2C1_RX    = 0x14,
        Channel5_TIM1_UP    = 0x14,
        Channel5_TIM2_CH1   = 0x14,        
        Channel5_DAC2_CH1   = 0x14,        
        Channel5_TIM15_UP   = 0x14,
        Channel5_TIM15_TRIG = 0x14,
        Channel5_TIM15_COM  = 0x14,
        
        Channel6_USART2_RX  = 0x15,
        Channel6_I2C1_TX    = 0x15,
        Channel6_TIM3_CH1   = 0x15,
        Channel6_TIM3_TRIG  = 0x15,
        Channel6_TIM16_CH1  = 0x15,
        Channel6_TIM16_UP   = 0x15, 
        Channel6_SPI1_RX    = 0x15,
        
        Channel7_USART2_TX  = 0x16,
        Channel7_I2C1_RX    = 0x16,        
        Channel7_TIM2_CH2   = 0x16,
        Channel7_TIM2_CH4   = 0x16,      
        Channel7_TIM17_CH1  = 0x16,
        Channel7_TIM17_UP   = 0x16,
        Channel1_SPI1_TX    = 0x16,       

};

#endif