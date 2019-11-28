#ifndef _DMA4XX_H
#define _DMA4XX_H

#include "stm32_conf.h"
#include "core/coreexception.h"
#include "core/coretypes.h"

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
        ChannelDac1,
        ChannelDac2,
        ChannelAdc1,
        ChannelAdc2,
        ChannelAdc3,
        ChannelUsart1_Rx,
        ChannelUsart1_Tx,
        ChannelUsart2_Rx,
        ChannelUsart2_Tx,
        ChannelUsart3_Rx,
        ChannelUsart3_Tx,
        ChannelUsart4_Rx,
        ChannelUsart4_Tx,
        ChannelUsart5_Rx,
        ChannelUsart5_Tx,
        ChannelUsart6_Rx,
        ChannelUsart6_Tx,
        ChannelSpi1_Rx,
        ChannelSpi1_Tx,
        ChannelSpi2_Rx,
        ChannelSpi2_Tx,
        ChannelSpi3_Rx,
        ChannelSpi3_Tx,
    } DmaChannel;
  
private:
    static Dma *mStreams1[8];
    static Dma *mStreams2[8];
    DMA_Stream_TypeDef *mStream;
    uint32_t mChannel;
    uint32_t mDmaFlagMask;
    uint32_t mDmaFlagTc;
    DMA_InitTypeDef mConfig;
    
    bool mEnabled;
    bool mBufferConfigured;
    bool mPeriphConfigured;
    bool mConfigured;
    
    IRQn mIrq;
    NotifyEvent mOnTransferComplete;
    
    void tryInit();
    
    FOR_EACH_DMA(DECLARE_FRIEND)
    
    void handleInterrupt();
  
public:
    Dma(unsigned char stream, unsigned char channel);
    static Dma* getStreamForPeriph(DmaChannel channelName);
    ~Dma();
    
    void setSingleBuffer(void *buffer, int size);
    void setCircularBuffer(void *buffer, int size);
    void setDoubleBuffer(void *buffer, void *buffer2, int size);
    void setPeriph(void *periph, int dataSize, bool isSource);
    void setSource(void *periph, int dataSize);
    void setSink(void *periph, int dataSize);
    
    void start(int itemCount=0);
    void stop(bool wait=false);
    void setEnabled(bool enable);
    bool isEnabled() const {return mEnabled;}
    
    bool isComplete();
    
    int currentPage() const {return DMA_GetCurrentMemoryTarget(mStream);}
    inline int dataCounter() const {return mStream->NDTR;}
    
    void setTransferCompleteEvent(NotifyEvent event);
};

#endif
