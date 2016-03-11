#include "dma.h"

#define DMA_FLAG_MASK(i) (DMA_FLAG_TCIF##i | DMA_FLAG_HTIF##i | DMA_FLAG_TEIF##i | DMA_FLAG_DMEIF##i | DMA_FLAG_FEIF##i)
#define DMA_FLAG_TC(i) (DMA_FLAG_TCIF##i)

#define DMA_STREAM(x,y) DMA##x##_Stream##y
#define DMA_IRQn(x,y) DMA##x##_Stream##y##_IRQn

Dma *Dma::mStreams1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
Dma *Dma::mStreams2[8] = {0, 0, 0, 0, 0, 0, 0, 0};

Dma::Dma(unsigned char stream, unsigned char channel) :
    mDmaFlagMask(false),
    mBufferConfigured(false),
    mPeriphConfigured(false),
    mConfigured(false)
{
    switch (stream)
    {
        #define DMA_STREAM_INIT(x,y) \
        case x##y: \
            mStream = DMA_STREAM(x,y); \
            mDmaFlagMask = DMA_FLAG_MASK(y); \
            mDmaFlagTc = DMA_FLAG_TC(y); \
            mIrq = DMA_IRQn(x,y); \
            break;
            
        FOR_EACH_DMA(DMA_STREAM_INIT)
    }
    
    if (stream < 20)
    {
        unsigned char idx = stream - 10;
        if (mStreams1[idx])
            throw Exception::resourceBusy;
        mStreams1[idx] = this;
    }
    else
    {
        unsigned char idx = stream - 20;
        if (mStreams2[idx])
            throw Exception::resourceBusy;
        mStreams2[idx] = this;
    }
    
    switch (channel)
    {
        case 0: mChannel = DMA_Channel_0; break;
        case 1: mChannel = DMA_Channel_1; break;
        case 2: mChannel = DMA_Channel_2; break;
        case 3: mChannel = DMA_Channel_3; break;
        case 4: mChannel = DMA_Channel_4; break;
        case 5: mChannel = DMA_Channel_5; break;
        case 6: mChannel = DMA_Channel_6; break;
        case 7: mChannel = DMA_Channel_7; break;
    }
    
    if (stream >= 10 && stream <= 17)
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    else if (stream >= 20 && stream <= 27)
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    
    DMA_StructInit(&mConfig);
    
    mConfig.DMA_Channel = mChannel;
    //mConfig.DMA_PeripheralBaseAddr = ;
    //mConfig.DMA_Memory0BaseAddr = ;
    //mConfig.DMA_DIR = ;
    //mConfig.DMA_BufferSize = ;
    mConfig.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    mConfig.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //mConfig.DMA_PeripheralDataSize = ;
    //mConfig.DMA_MemoryDataSize = ;
    mConfig.DMA_Mode = DMA_Mode_Circular;
    mConfig.DMA_Priority = DMA_Priority_High;
    mConfig.DMA_FIFOMode = DMA_FIFOMode_Disable;
    mConfig.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    mConfig.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    mConfig.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
}

Dma* Dma::getStreamForPeriph(DmaChannel channelName)
{
    switch (channelName)
    {
        case ChannelDac1:       return new Dma(15, 7);
        case ChannelDac2:       return new Dma(16, 7);
        case ChannelAdc:        return new Dma(24, 0);
        case ChannelUsart1_Rx:  return new Dma(25, 4); // alternative (22, 4)
        case ChannelUsart1_Tx:  return new Dma(27, 4); 
        case ChannelUsart2_Rx:  return new Dma(15, 4);
        case ChannelUsart2_Tx:  return new Dma(16, 4);
        case ChannelUsart3_Rx:  return new Dma(11, 4);
        case ChannelUsart3_Tx:  return new Dma(13, 4); // alternative (14, 7);
        case ChannelUsart4_Rx:  return new Dma(12, 4);
        case ChannelUsart4_Tx:  return new Dma(14, 4);
        case ChannelUsart5_Rx:  return new Dma(10, 4);
        case ChannelUsart5_Tx:  return new Dma(17, 4);
        case ChannelUsart6_Rx:  return new Dma(21, 5); // alternative (22, 5)
        case ChannelUsart6_Tx:  return new Dma(26, 5); // alternative (27, 5)
        case ChannelSpi1_Rx:    return new Dma(20, 3); // alternative (22, 3)
        case ChannelSpi1_Tx:    return new Dma(23, 3); // alternative (25, 3)
        case ChannelSpi2_Rx:    return new Dma(13, 0);
        case ChannelSpi2_Tx:    return new Dma(14, 0);
        case ChannelSpi3_Rx:    return new Dma(10, 0); // alternative (12, 0)
        case ChannelSpi3_Tx:    return new Dma(17, 0); // alternative (15, 0)
    }
    return 0L;
}

Dma::~Dma()
{
    DMA_Cmd(mStream, DISABLE);
    DMA_DeInit(mStream);
}
//---------------------------------------------------------------------------

void Dma::tryInit()
{
    if (mBufferConfigured && mPeriphConfigured)
    {
        DMA_Init(mStream, &mConfig);
        mConfigured = true;
    }
}
//---------------------------------------------------------------------------

void Dma::setSingleBuffer(void *buffer, int size)
{
    mConfig.DMA_Memory0BaseAddr = (uint32_t)buffer;
    mConfig.DMA_BufferSize = size; 
    mConfig.DMA_Mode = DMA_Mode_Normal;
    mBufferConfigured = true;
    tryInit();
    
    DMA_DoubleBufferModeCmd(mStream, DISABLE);
}

void Dma::setCircularBuffer(void *buffer, int size)
{
    mConfig.DMA_Memory0BaseAddr = (uint32_t)buffer;
    mConfig.DMA_BufferSize = size; 
    mConfig.DMA_Mode = DMA_Mode_Circular;
    mBufferConfigured = true;
    tryInit();
    
    DMA_DoubleBufferModeCmd(mStream, DISABLE);
}

void Dma::setDoubleBuffer(void *buffer, void *buffer2, int size)
{
    mConfig.DMA_Memory0BaseAddr = (uint32_t)buffer;
    mConfig.DMA_BufferSize = size;
    mBufferConfigured = true;
    tryInit();
    
    DMA_DoubleBufferModeConfig(mStream, (uint32_t)buffer2, DMA_Memory_0);
    DMA_DoubleBufferModeCmd(mStream, ENABLE);
}

void Dma::setPeriph(void *periph, int dataSize, bool isSource)
{
    uint32_t periphDataSize = 0;
    uint32_t memoryDataSize = 0;
    switch (dataSize)
    {
      case 1:
        periphDataSize = DMA_PeripheralDataSize_Byte;
        memoryDataSize = DMA_MemoryDataSize_Byte;
        break;
        
      case 2:
        periphDataSize = DMA_PeripheralDataSize_HalfWord;
        memoryDataSize = DMA_MemoryDataSize_HalfWord;
        break;
        
      case 4:
        periphDataSize = DMA_PeripheralDataSize_Word;
        memoryDataSize = DMA_MemoryDataSize_Word;
        break;
    }
  
    mConfig.DMA_PeripheralBaseAddr = (uint32_t)periph;
    mConfig.DMA_DIR = isSource? DMA_DIR_PeripheralToMemory : DMA_DIR_MemoryToPeripheral;
    mConfig.DMA_PeripheralDataSize = periphDataSize;
    mConfig.DMA_MemoryDataSize = memoryDataSize;
    mPeriphConfigured = true;
    tryInit();
}

void Dma::setSource(void *periph, int dataSize)
{
    setPeriph(periph, dataSize, true);
}

void Dma::setSink(void *periph, int dataSize)
{
    setPeriph(periph, dataSize, false);
}
//---------------------------------------------------------------------------

void Dma::start(int itemCount)
{
    if (itemCount)
    {
        DMA_SetCurrDataCounter(mStream, itemCount);
        // clearing flags is necessary for enabling dma streaming
        DMA_ClearFlag(mStream, mDmaFlagMask);
    }
    setEnabled(true);
}

void Dma::stop(bool wait)
{
    setEnabled(false);
    if (wait)
        DMA_GetCmdStatus(mStream);
    if (mStream == DMA1_Stream5)
        DMA_ClearFlag(mStream, DMA_FLAG_FEIF5);
    
}

void Dma::setEnabled(bool enable)
{
    if (mConfigured && enable)
    {
        DMA_Cmd(mStream, ENABLE);
        mEnabled = true;
    }
    else
    {
        DMA_Cmd(mStream, DISABLE);
        mEnabled = false;
    }
}
//---------------------------------------------------------------------------

void Dma::setTransferCompleteEvent(NotifyEvent event)
{
    mOnTransferComplete = event;
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = mIrq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    DMA_ClearFlag(mStream, mDmaFlagMask);
    DMA_ITConfig(mStream, DMA_IT_TC, ENABLE);
}
//---------------------------------------------------------------------------

void Dma::handleInterrupt()
{
    bool sts = DMA_GetFlagStatus(mStream, mDmaFlagTc);
    DMA_ClearITPendingBit(mStream, mDmaFlagMask);
    
    if (sts && mOnTransferComplete)
        mOnTransferComplete();
    
}
//---------------------------------------------------------------------------


#ifdef __cplusplus
 extern "C" {
#endif 
   
#define DEFINE_DMA_IRQ_HANDLER(x,y) \
    void DMA##x##_Stream##y##_IRQHandler() {Dma::mStreams##x[y]->handleInterrupt();}
    
FOR_EACH_DMA(DEFINE_DMA_IRQ_HANDLER)    
  
#ifdef __cplusplus
}
#endif