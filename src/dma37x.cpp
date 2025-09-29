#if defined(STM32F37X)

#include "dma37x.h"

Dma *Dma::mDma1Channels[7] = {0, 0, 0, 0, 0, 0, 0};
Dma *Dma::mDma2Channels[5] = {0, 0, 0, 0, 0};

Dma::Dma(DmaChannel channelName)
{
    mChannel = channelName & 0x07;
    switch (channelName & 0xF0)
    {
      case 0x10:
        mDma = DMA1;
        mIrq = (IRQn)(DMA1_Channel1_IRQn + mChannel);
        mDma1Channels[mChannel] = this;
        if (mChannel >= 7)
            throw Exception::invalidPeriph;
        RCC->AHBENR |= RCC_AHBENR_DMA1EN; // enable clock
        break;
      case 0x20:
        mDma = DMA2;
        mIrq = (IRQn)(DMA2_Channel1_IRQn + mChannel);
        mDma2Channels[mChannel] = this;
        if (mChannel >= 5)
            throw Exception::invalidPeriph;
        RCC->AHBENR |= RCC_AHBENR_DMA2EN; // enable clock
        break;
      default: throw Exception::invalidPeriph;
    }   
    mDmaChan = mDma->channel + mChannel;
}

Dma::~Dma()
{
    mDmaChan->CCR &= ~DMA_CCR_EN;
}
//---------------------------------------------------------------------------

void Dma::setSingleBuffer(void *buffer, int size)
{
    mDmaChan->CMAR = (unsigned long)buffer; // set memory address
    mDmaChan->CNDTR = size; // set transfer byte count
    mDmaChan->CCR &= ~DMA_CCR_CIRC; // NOT circular buffer
    mDmaChan->CCR |= DMA_CCR_MINC; // memory increment
}

void Dma::setCircularBuffer(void *buffer, int size)
{
    mDmaChan->CMAR = (unsigned long)buffer; // set memory address
    mDmaChan->CNDTR = size; // set transfer byte count
    mDmaChan->CCR |= DMA_CCR_CIRC; // circular buffer
    mDmaChan->CCR |= DMA_CCR_MINC; // memory increment
}

void Dma::setSource(void *periph, int dataSize)
{
    mDmaChan->CPAR = (unsigned long)periph; // set periph address
    mDmaChan->CCR &= ~DMA_CCR_PINC; // periph increment OFF
    mDmaChan->CCR &= ~DMA_CCR_PSIZE; // clear periph size
    mDmaChan->CCR &= ~DMA_CCR_MSIZE; // clear memory size
    mDmaChan->CCR |= ((dataSize>>1) << 8) & DMA_CCR_PSIZE; // set proper periph size
    mDmaChan->CCR |= ((dataSize>>1) << 10) & DMA_CCR_MSIZE; // set proper memory size
    mDmaChan->CCR &= ~(DMA_CCR_DIR); // from peripheral to memory
}

void Dma::setSink(void *periph, int dataSize)
{
    mDmaChan->CPAR = (unsigned long)periph; // set periph address
    mDmaChan->CCR &= ~DMA_CCR_PINC; // periph increment OFF
    mDmaChan->CCR &= ~DMA_CCR_PSIZE; // clear periph size
    mDmaChan->CCR &= ~DMA_CCR_MSIZE; // clear memory size
    mDmaChan->CCR |= ((dataSize>>1) << 8) & DMA_CCR_PSIZE; // set proper periph size
    mDmaChan->CCR |= ((dataSize>>1) << 10) & DMA_CCR_MSIZE; // set proper memory size
    mDmaChan->CCR |= (DMA_CCR_DIR); // from memory to peripheral
}
//---------------------------------------------------------------------------

void Dma::start(int size)
{
    if (size)
    {
        mDmaChan->CNDTR = size; // set transfer byte count
    }
    // clearing flags is necessary for enabling dma streaming
    mDma->status.IFCR = (DMA_IFCR_CGIF1 << (mChannel*4)); // reset global flag for the channel
    mDmaChan->CCR |= DMA_CCR_EN; // enable channel
}

void Dma::stop(bool wait)
{
    if (mDmaChan->CCR & DMA_CCR_EN) // if channel is enabled
    {
        mDmaChan->CCR &= ~DMA_CCR_EN; // disable channel
        if (wait && mDmaChan->CNDTR) // wait if there is unsent data
        {
            // wait for Transfer Complete flag status:
            while (!(mDma->status.ISR & (DMA_ISR_TCIF1 << (mChannel*4))));
        }
    }
}

void Dma::setEnabled(bool enable)
{
    if (enable)
        mDmaChan->CCR |= DMA_CCR_EN;
    else
        mDmaChan->CCR &= ~DMA_CCR_EN;
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
    
    mDma->status.IFCR = (DMA_IFCR_CGIF1 << (mChannel*4)); // reset global flag for the channel
    mDmaChan->CCR |= DMA_CCR_TCIE; // enable Transfer Complete interrupt
}
//---------------------------------------------------------------------------

void Dma::handleInterrupt()
{
    bool sts = mDma->status.ISR & (DMA_ISR_TCIF1 << (mChannel*4)); // get Transfer Complete flag status
    //mDma->status.IFCR = (DMA_IFCR_CTCIF1 << (mChannel*4)); // reset Transfer Complete flag
    mDma->status.IFCR = (DMA_IFCR_CGIF1 << (mChannel*4)); // reset global status flag
    
    if (sts && mOnTransferComplete)
        mOnTransferComplete(); 
}
//---------------------------------------------------------------------------


#ifdef __cplusplus
 extern "C" {
#endif 
   
#define DEFINE_DMA_IRQ_HANDLER(x,y) \
void DMA##x##_Channel##y##_IRQHandler() \
{ \
    Dma::mDma##x##Channels[y-1]->handleInterrupt(); \
}
    
FOR_EACH_DMA(DEFINE_DMA_IRQ_HANDLER)    
  
#ifdef __cplusplus
}
#endif

#endif