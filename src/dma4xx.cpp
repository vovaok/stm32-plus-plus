#include "dma4xx.h"

#define DMA_FLAG_MASK(i) (DMA_FLAG_TCIF##i | DMA_FLAG_HTIF##i | DMA_FLAG_TEIF##i | DMA_FLAG_DMEIF##i | DMA_FLAG_FEIF##i)
#define DMA_FLAG_TC(i) (DMA_FLAG_TCIF##i)

#define DMA_STREAM(x,y) DMA##x##_Stream##y
#define DMA_IRQn(x,y) DMA##x##_Stream##y##_IRQn,

Dma *Dma::mStreams[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

Dma::Dma(Channel channelName)
{
    int dma_num = channelName >> 8;
    mStreamNum  = (channelName >> 4) & 7;
    mChannelNum = channelName & 7;
      
    int idx = mStreamNum + 8*(dma_num - 1);
    if (mStreams[idx])
        THROW(Exception::ResourceBusy);
    mStreams[idx] = this;
    
    if (dma_num == 1)
    {
        mDma = DMA1;
        mStream = DMA1_Stream0 + mStreamNum;
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    }
    if (dma_num == 2)
    {
        mDma = DMA2;
        mStream = DMA2_Stream0 + mStreamNum;
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    }
    
    const IRQn_Type irq[16] = {FOR_EACH_DMA(DMA_IRQn)};
    mIrq = irq[idx];
    
    mConfig.all = 0;
}

Dma::~Dma()
{
    mStream->CR &= ~DMA_SxCR_EN;
    mStream->CR = 0;
    mStream->NDTR = 0;
    mStream->PAR = 0;
    mStream->M0AR = 0;
    mStream->M1AR = 0;
    mStream->FCR = 0x00000021;
    clearFlag(AllFlags);
}
//---------------------------------------------------------------------------

bool Dma::testFlag(uint32_t flag) const
{
    if (mStreamNum & 1)
        flag <<= 6;
    if (mStreamNum & 2)
        flag <<= 16;
    if (mStreamNum & 4)
        return mDma->HISR & flag;
    else
        return mDma->LISR & flag;
}

void Dma::clearFlag(uint32_t flag)
{
    if (mStreamNum & 1)
        flag <<= 6;
    if (mStreamNum & 2)
        flag <<= 16;
    if (mStreamNum & 4)
        mDma->HIFCR = flag;
    else
        mDma->LIFCR = flag;
}
//---------------------------------------------------------------------------

void Dma::setSingleBuffer(void *buffer, int size)
{
    mConfig.DBM = 0;
    mConfig.MINC = 1;
    mConfig.CIRC = 0;

    mStream->CR = mConfig.all;
    mStream->NDTR = size;
    mStream->M0AR = (uint32_t)buffer;
}

void Dma::setCircularBuffer(void *buffer, int size)
{
    mConfig.DBM = 0;
    mConfig.MINC = 1;
    mConfig.CIRC = 1;
    
    mStream->CR = mConfig.all;
    mStream->NDTR = size;
    mStream->M0AR = (uint32_t)buffer;
}

void Dma::setDoubleBuffer(void *buffer, void *buffer2, int size)
{
    mConfig.DBM = 1;
    mConfig.MINC = 1;
    mConfig.CIRC = 0;
    
    mStream->CR = mConfig.all;
    mStream->NDTR = size;
    mStream->M0AR = (uint32_t)buffer;
    mStream->M1AR = (uint32_t)buffer2;
}

void Dma::setMemorySource(void *ptr, int dataSize)
{
    if (dataSize == 4)
        --dataSize;
    --dataSize;
    
    mConfig.DBM = 0;
    mConfig.MINC = 0;
    mConfig.CIRC = 0;
    mConfig.MSIZE = dataSize;
    
    mStream->CR = mConfig.all;
    mStream->M0AR = (uint32_t)ptr;
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
    
    mStream->PAR = (uint32_t)periph;
    mStream->CR = mConfig.all;
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
    mConfig.CHSEL = mChannelNum;
    mConfig.MBURST = 0;
    mConfig.PBURST = 0;
    mConfig.CT = 0;
    mConfig.PL = 2; // priority level high
    mConfig.PINCOS = 0;
    
    mStream->CR = mConfig.all;
    
    if (size)
        mStream->NDTR = size;
    // clearing flags is necessary for enabling dma streaming
    clearFlag(AllFlags);
    // enable the stream
    mStream->CR |= DMA_SxCR_EN;
}

void Dma::stop(bool wait)
{
    // if stream is enabled
    if (mStream->CR & DMA_SxCR_EN)
    {
        // disable the stream
        mStream->CR &= ~DMA_SxCR_EN;
        // wait for transfer complete
        while (wait && mStream->NDTR && !testFlag(TCIF));
    }
//    if (mStream == DMA1_Stream5) // WUT??
//        DMA_ClearFlag(mStream, DMA_FLAG_FEIF5); // ???
}

void Dma::setEnabled(bool enabled)
{
    if (enabled) 
        mStream->CR |= DMA_SxCR_EN;
    else
        mStream->CR &= ~DMA_SxCR_EN;
}

bool Dma::isEnabled() const
{
    return mStream->CR & DMA_SxCR_EN;
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
    mStream->CR = mConfig.all;
}

bool Dma::isComplete()
{
    bool result = testFlag(TCIF);
    if (result)
        clearFlag(TCIF);
    return result;
}

int Dma::currentPage() const
{
    return (mStream->CR & DMA_SxCR_CT)? 1: 0;
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
    void DMA##x##_Stream##y##_IRQHandler() {Dma::mStreams[8*(x-1)+y]->handleInterrupt();}
    
FOR_EACH_DMA(DEFINE_DMA_IRQ_HANDLER)    
  
#ifdef __cplusplus
}
#endif