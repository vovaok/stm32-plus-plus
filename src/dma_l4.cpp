#include "dma_l4.h"

#define DMA_FLAG_MASK(i) (DMA_FLAG_GIF##i | DMA_FLAG_TCIF##i | DMA_FLAG_HTIF##i | DMA_FLAG_TEIF##i)
#define DMA_FLAG_TC(i) (DMA_FLAG_TCIF##i)

#define DMA_CHANNEL(x,y) DMA##x##_Channel##y
#define DMA_IRQn(x,y) DMA##x##_Channel##y##_IRQn,

Dma *Dma::mChannels[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

Dma::Dma(Channel channelName)
{
    int dma_num = channelName >> 8;
    mChannelNum  = ((channelName >> 4) & 7) - 1; // 0...6
    mChannelSel = channelName & 7;
      
    int idx = mChannelNum + 8*(dma_num - 1);
    if (mChannels[idx])
        THROW(Exception::ResourceBusy);
    mChannels[idx] = this;
    
    if (dma_num == 1)
    {
        mDma = DMA1;
        mChannel = DMA1_Channel1 + mChannelNum;
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
        DMA1_CSELR->CSELR |= mChannelSel << (mChannelNum*4);
    }
    if (dma_num == 2)
    {
        mDma = DMA2;
        mChannel = DMA2_Channel1 + mChannelNum;
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
        DMA2_CSELR->CSELR |= mChannelSel << (mChannelNum*4);
    }
    
    const IRQn_Type irq[14] = {FOR_EACH_DMA(DMA_IRQn)};
    mIrq = irq[idx];
    
    mConfig.all = 0;
}

Dma::~Dma()
{
    mChannel->CCR &= ~DMA_CCR_EN;
    mChannel->CCR = 0;
    mChannel->CNDTR = 0;
    mChannel->CPAR = 0;
    mChannel->CMAR = 0;
    clearFlag(AllFlags);
}
//---------------------------------------------------------------------------

bool Dma::testFlag(uint32_t flag) const
{
    flag <<= 4 * mChannelNum;
    return mDma->ISR & flag;
}

void Dma::clearFlag(uint32_t flag)
{
    flag <<= 4 * mChannelNum;
    mDma->IFCR = flag;
}
//---------------------------------------------------------------------------

void Dma::setSingleBuffer(void *buffer, int size)
{
    mConfig.MINC = 1;
    mConfig.CIRC = 0;

    mChannel->CCR = mConfig.all;
    mChannel->CNDTR = size;
    mChannel->CMAR = (uint32_t)buffer;
}

void Dma::setCircularBuffer(void *buffer, int size)
{
    mConfig.MINC = 1;
    mConfig.CIRC = 1;
    
    mChannel->CCR = mConfig.all;
    mChannel->CNDTR = size;
    mChannel->CMAR = (uint32_t)buffer;
}

void Dma::setMemorySource(void *ptr, int dataSize)
{
    if (dataSize == 4)
        --dataSize;
    --dataSize;
    
    mConfig.MINC = 0;
    mConfig.CIRC = 0;
    mConfig.MSIZE = dataSize;
    
    mChannel->CCR = mConfig.all;
    mChannel->CMAR = (uint32_t)ptr;
}

void Dma::setMemorySource(uint8_t *ptr)
{
    setMemorySource(ptr, 1);
}

void Dma::setMemorySource(uint16_t *ptr)
{
    setMemorySource(ptr, 2);
}

void Dma::setMemorySource(uint32_t *ptr)
{
    setMemorySource(ptr, 4);
}

void Dma::setPeriph(void *periph, int dataSize, bool isSource)
{
    if (dataSize == 4)
        --dataSize;
    --dataSize;
    
    mConfig.PSIZE = dataSize;
    mConfig.MSIZE = dataSize;
    mConfig.DIR = isSource? 0: 1;
    
    mChannel->CPAR = (uint32_t)periph;
    mChannel->CCR = mConfig.all;
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

void Dma::start(int size)
{
    mConfig.PL = 2; // priority level high
    
    mChannel->CCR = mConfig.all;
    
    if (size)
        mChannel->CNDTR = size;
    // clearing flags is necessary for enabling dma streaming
    clearFlag(AllFlags);
    // enable the stream
    mChannel->CCR |= DMA_CCR_EN;
}

void Dma::stop(bool wait)
{
    // if stream is enabled
    if (mChannel->CCR & DMA_CCR_EN)
    {
        // disable the stream
        mChannel->CCR &= ~DMA_CCR_EN;
        // wait for transfer complete
        if (wait && mChannel->CNDTR)
        {
            while (!testFlag(TCIF));
        }
    }
//    if (mChannel == DMA1_Stream5) // WUT??
//        DMA_ClearFlag(mChannel, DMA_FLAG_FEIF5); // ???
}

void Dma::setEnabled(bool enabled)
{
    if (enabled) 
        mChannel->CCR |= DMA_CCR_EN;
    else
        mChannel->CCR &= ~DMA_CCR_EN;
}

bool Dma::isEnabled() const
{
    return mChannel->CCR & DMA_CCR_EN;
}
//---------------------------------------------------------------------------

void Dma::setTransferCompleteEvent(NotifyEvent event)
{
    mOnTransferComplete = event;
    
//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = mIrq;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_EnableIRQ(mIrq);
    
    clearFlag(AllFlags);
    // enable Transfer Complete interrupt
    mConfig.TCIE = 1;
    mChannel->CCR = mConfig.all;
}

bool Dma::isComplete()
{
    bool result = testFlag(TCIF);
    if (result)
        clearFlag(TCIF);
    return result;
}
//---------------------------------------------------------------------------

void Dma::handleInterrupt()
{
    bool sts = testFlag(TCIF);
    clearFlag(AllFlags);
    
    if (sts && mOnTransferComplete)
        mOnTransferComplete();
}
//---------------------------------------------------------------------------


#ifdef __cplusplus
 extern "C" {
#endif 
   
#define DEFINE_DMA_IRQ_HANDLER(x,y) \
    void DMA##x##_Channel##y##_IRQHandler() {Dma::mChannels[7*(x-1)+(y-1)]->handleInterrupt();}
    
FOR_EACH_DMA(DEFINE_DMA_IRQ_HANDLER)    
  
#ifdef __cplusplus
}
#endif