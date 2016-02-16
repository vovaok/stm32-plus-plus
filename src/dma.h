#ifndef _DMA_H
#define _DMA_H

#include "stm32_conf.h"

class Dma
{
public:
    typedef enum
    {
        ChannelDac1,
        ChannelDac2,
        ChannelAdc,
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
    DMA_Stream_TypeDef *mStream;
    uint32_t mChannel;
    uint32_t mDmaFlagMask;
    DMA_InitTypeDef mConfig;
    
    bool mEnabled;
    bool mBufferConfigured;
    bool mPeriphConfigured;
    bool mConfigured;
    
    void tryInit();
  
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
    
    int currentPage() const {return DMA_GetCurrentMemoryTarget(mStream);}
    inline int dataCounter() const {return mStream->NDTR;}
};

#endif