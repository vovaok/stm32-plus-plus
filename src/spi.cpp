#include "spi.h"

Spi *Spi::mSpies[6] = {0L, 0L, 0L, 0L, 0L, 0L};

Spi::Spi(Gpio::Config sck, Gpio::Config miso, Gpio::Config mosi) :
    mDev(0L)//,
//    mUseDmaRx(false), mUseDmaTx(false),
//    mDmaRx(0L),
//    mDmaTx(0L)
{
    int no = GpioConfigGetPeriphNumber(sck);
    if (no == 0)
        THROW(Exception::InvalidPin);
    if (miso != Gpio::NoConfig && no != GpioConfigGetPeriphNumber(miso))
        THROW(Exception::InvalidPin);
    if (mosi != Gpio::NoConfig && no != GpioConfigGetPeriphNumber(mosi))
        THROW(Exception::InvalidPin);
  
    mSpies[no-1] = this;
    
    Gpio::config(sck);
    Gpio::config(miso);
    Gpio::config(mosi);
    
    switch (no)
    {
      case 1:
        mDev = SPI1;
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
        mIrq = SPI1_IRQn;
//        mDmaChannelRx = Dma::ChannelSpi1_Rx;
//        mDmaChannelTx = Dma::ChannelSpi1_Tx;
        break;
        
      case 2:
        mDev = SPI2;
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
        mIrq = SPI2_IRQn;
//        mDmaChannelRx = Dma::ChannelSpi2_Rx;
//        mDmaChannelTx = Dma::ChannelSpi2_Tx;
        break;
        
      case 3:
        mDev = SPI3;
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
        mIrq = SPI3_IRQn;
//        mDmaChannelRx = Dma::ChannelSpi3_Rx;
//        mDmaChannelTx = Dma::ChannelSpi3_Tx;
        break;
        
#if defined(STM32F429XX) 
      case 4:
        mDev = SPI4;
        RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
        mIrq = SPI4_IRQn;
//        mDmaChannelRx = Dma::ChannelSpi4_Rx;
//        mDmaChannelTx = Dma::ChannelSpi4_Tx;
        break;
        
      case 5:
        mDev = SPI5;
        RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
        mIrq = SPI5_IRQn;
//        mDmaChannelRx = Dma::ChannelSpi5_Rx;
//        mDmaChannelTx = Dma::ChannelSpi5_Tx;
        break;
        
      case 6:
        mDev = SPI6;
        RCC->APB2ENR |= RCC_APB2ENR_SPI6EN;
        mIrq = SPI6_IRQn;
//        mDmaChannelRx = Dma::ChannelSpi6_Rx;
//        mDmaChannelTx = Dma::ChannelSpi6_Tx;
        break;
#endif
    }
    
    if (!mDev)
        THROW(Exception::InvalidPeriph);
}
//---------------------------------------------------------------------------

void Spi::setConfig(Config cfg)
{
    bool en = mConfig.enable;
    cfg.enable = 0;
    mConfig = cfg;
    mConfig.SSI = 1;
    mConfig.SSM = 1;
    mDev->CR1 = mConfig.word;
    if (en)
    {
        mConfig.enable = 1;
        mDev->CR1 = mConfig.word;
    }
}

void Spi::setMasterMode()
{
    mConfig.master = 1;
    setConfig(mConfig);
}

void Spi::setCPOL_CPHA(bool CPOL, bool CPHA)
{
    mConfig.CPOL = CPOL? 1: 0;
    mConfig.CPHA = CPHA? 1: 0;
    setConfig(mConfig);
}

void Spi::setBaudratePrescaler(int psc)
{
    mConfig.baudrate = psc & 7;
    setConfig(mConfig);
}
//--------------------------------------------------------------------------

void Spi::open()
{
    if (isOpen())
        return;// false;
    
//    if (mUseDmaRx)
//    {
//        if (!mDmaRx)
//            mDmaRx = Dma::getStreamForPeriph(mDmaChannelRx);
//        mDmaRx->setSource((void*)&mDev->DR, 1);
//        mDev->CR2 |= SPI_CR2_RXDMAEN;
//    }
//    
//    if (mUseDmaTx)
//    {
//        if (!mDmaTx)
//            mDmaTx = Dma::getStreamForPeriph(mDmaChannelTx);
//        mDmaTx->setSink((void*)&mDev->DR, 1);
//        mDev->CR2 |= SPI_CR2_TXDMAEN;
//    }   
    
    mConfig.enable = 1;
    mDev->CR1 = mConfig.word;    
}

void Spi::close()
{
//    if (mDmaRx)
//    {
//        mDev->CR2 &= ~SPI_CR2_RXDMAEN;
//        mDmaRx->stop(true);
//        delete mDmaRx;
//        mDmaRx = 0L;
//    }
//    if (mDmaTx)
//    {
//        mDev->CR2 &= ~SPI_CR2_TXDMAEN;
//        mDmaTx->stop(true);
//        delete mDmaTx;
//        mDmaTx = 0L;
//    }
    
    mConfig.enable = 0;
    mDev->CR1 = mConfig.word;
}
//---------------------------------------------------------------------------

//uint16_t Spi::transferWord(uint16_t word)
//{
//    //while (!(mDev->SR & SPI_SR_TXE));
//    mDev->DR = word;
//    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
////    while (mDev->SR & SPI_SR_BSY); // wait while SPI is busy
//    return mDev->DR;
//}

void Spi::transferWordAsync(uint16_t word)
{
    mDev->DR = word;
}

void Spi::transfer(uint8_t* data, int size)
{
//    if (!mUseDmaRx && !mUseDmaTx)
//    {
        for (int i=0; i<size; i++)
        {
            mDev->DR = data[i];
            while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
            data[i] =  mDev->DR;
        }
//    }
//    else
//    {
//        if (mUseDmaRx)
//        {
//            mDmaRx->setSingleBuffer(data, size);
//            mDmaRx->start();
//        }
//        if (mUseDmaTx)
//        {
//            mDmaTx->setSingleBuffer(data, size);
//            mDmaTx->start();
//        }
//    }
}

void Spi::transfer(const uint8_t *data, uint8_t *buffer, int size)
{
    while (size--)
    {
        mDev->DR = *data++;
        while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
        *buffer++ = mDev->DR;
    }
}

uint16_t Spi::read()
{
    mDev->DR = 0x00;
    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
    return mDev->DR;
}

uint16_t Spi::write(uint16_t word)
{
    mDev->DR = word;
    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
    return mDev->DR;
}

void Spi::read(uint8_t* data, int size)
{
    while (size--)
    {
        mDev->DR = 0x00;
        while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
        *data++ = mDev->DR;
    }
}

void Spi::write(const uint8_t *data, int size)
{
    while (size--)
    {
        mDev->DR = *data++;
        while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
        (void)mDev->DR;
    }
}

//---------------------------------------------------------------------------
//
//void Spi::setUseDmaRx(bool useDma)
//{
//    if (isOpen())
//        throw Exception::resourceBusy;
//    mUseDmaRx = useDma;
//}
//
//void Spi::setUseDmaTx(bool useDma)
//{
//    if (isOpen())
//        throw Exception::resourceBusy;
//    mUseDmaTx = useDma;
//}
//---------------------------------------------------------------------------

void Spi::setTransferCompleteEvent(SpiDataEvent e)
{
    onTransferComplete = e;
    enableInterrupt();
}
//---------------------------------------------------------------------------

void Spi::enableInterrupt()
{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = mIrq;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
    
    // compute priority
//    uint8_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
//    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700)) >> 0x08;
//    tmppre = (0x4 - tmppriority);
//    tmpsub = tmpsub >> tmppriority;
//    tmppriority = (3 << tmppre) | (2 & tmpsub);
//    NVIC->IP[mIrq] = tmppriority << 4;
//    NVIC->ISER[mIrq >> 5] = 1 << (mIrq & 0x1F);
  
    NVIC_EnableIRQ(mIrq);
    
    // enable RXNE interrupt
    mDev->CR2 |= SPI_CR2_RXNEIE;
}

void Spi::handleInterrupt()
{
    if (mDev->SR & SPI_SR_RXNE)
    {
        onTransferComplete(mDev->DR);
    }
}
//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif 

void SPI1_IRQHandler(void)
{
    if (Spi::mSpies[0])
        Spi::mSpies[0]->handleInterrupt();
}
   
void SPI2_IRQHandler(void)
{
    if (Spi::mSpies[1])
        Spi::mSpies[1]->handleInterrupt();
}

void SPI3_IRQHandler(void)
{
    if (Spi::mSpies[2])
        Spi::mSpies[2]->handleInterrupt();
}

#ifdef __cplusplus
}
#endif
