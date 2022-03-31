#include "spi.h"

Spi *Spi::mSpies[3] = {0L, 0L, 0L};

Spi::Spi(Gpio::Config sck, Gpio::Config miso, Gpio::Config mosi) :
    mUseDmaRx(false), mUseDmaTx(false),
    mDmaRx(0L),
    mDmaTx(0L)
{
    SpiNo no = getSpiByPin(sck);
    if (no == SpiNone)
        throw Exception::InvalidPin;
    if (miso != Gpio::NoConfig && no != getSpiByPin(miso))
        throw Exception::InvalidPin;
    if (mosi != Gpio::NoConfig && no != getSpiByPin(mosi))
        throw Exception::InvalidPin;
  
    mSpies[no-1] = this;
    
    Gpio::config(sck);
    Gpio::config(miso);
    Gpio::config(mosi);
    
    switch (no)
    {
      case Spi1:
        mDev = SPI1;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
        mIrq = SPI1_IRQn;
        mDmaChannelRx = Dma::ChannelSpi1_Rx;
        mDmaChannelTx = Dma::ChannelSpi1_Tx;
        break;
        
      case Spi2:
        mDev = SPI2;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        mIrq = SPI2_IRQn;
        mDmaChannelRx = Dma::ChannelSpi2_Rx;
        mDmaChannelTx = Dma::ChannelSpi2_Tx;
        break;
        
      case Spi3:
        mDev = SPI3;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
        mIrq = SPI3_IRQn;
        mDmaChannelRx = Dma::ChannelSpi3_Rx;
        mDmaChannelTx = Dma::ChannelSpi3_Tx;
        break;
    }
}


SpiNo Spi::getSpiByPin(Gpio::Config pin)
{
    switch (pin)
    {
        case Gpio::SPI1_MISO_PA6: case Gpio::SPI1_MISO_PB4: case Gpio::SPI1_MOSI_PA7: case Gpio::SPI1_MOSI_PB5:
        case Gpio::SPI1_NSS_PA15: case Gpio::SPI1_NSS_PA4: case Gpio::SPI1_SCK_PA5: case Gpio::SPI1_SCK_PB3:
        return Spi1;
        
        case Gpio::SPI2_MISO_PB14: case Gpio::SPI2_MISO_PC2: case Gpio::SPI2_MISO_PI2: case Gpio::SPI2_MOSI_PB15:
        case Gpio::SPI2_MOSI_PC3: case Gpio::SPI2_MOSI_PI3: case Gpio::SPI2_NSS_PB12: case Gpio::SPI2_NSS_PB9: 
        case Gpio::SPI2_NSS_PI0: case Gpio::SPI2_SCK_PB10: case Gpio::SPI2_SCK_PB13: case Gpio::SPI2_SCK_PI1:
        return Spi2;
        
        case Gpio::SPI3_MISO_PB4: case Gpio::SPI3_MISO_PC11: case Gpio::SPI3_MOSI_PB5: case Gpio::SPI3_MOSI_PC12:
        case Gpio::SPI3_NSS_PA15: case Gpio::SPI3_NSS_PA4: case Gpio::SPI3_SCK_PB3: case Gpio::SPI3_SCK_PC10:
        return Spi3;

        default: return SpiNone;
    }
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
//--------------------------------------------------------------------------

void Spi::open()
{
    if (isOpen())
        return;// false;
    
    if (mUseDmaRx)
    {
        if (!mDmaRx)
            mDmaRx = Dma::getStreamForPeriph(mDmaChannelRx);
        mDmaRx->setSource((void*)&mDev->DR, 1);
        mDev->CR2 |= SPI_CR2_RXDMAEN;
    }
    
    if (mUseDmaTx)
    {
        if (!mDmaTx)
            mDmaTx = Dma::getStreamForPeriph(mDmaChannelTx);
        mDmaTx->setSink((void*)&mDev->DR, 1);
        mDev->CR2 |= SPI_CR2_TXDMAEN;
    }   
    
    mConfig.enable = 1;
    mDev->CR1 = mConfig.word;    
}

void Spi::close()
{
    if (mDmaRx)
    {
        mDev->CR2 &= ~SPI_CR2_RXDMAEN;
        mDmaRx->stop(true);
        delete mDmaRx;
        mDmaRx = 0L;
    }
    if (mDmaTx)
    {
        mDev->CR2 &= ~SPI_CR2_TXDMAEN;
        mDmaTx->stop(true);
        delete mDmaTx;
        mDmaTx = 0L;
    }
    
    mConfig.enable = 0;
    mDev->CR1 = mConfig.word;
}
//---------------------------------------------------------------------------

unsigned short Spi::transferWord(unsigned short word)
{
    //while (!(mDev->SR & SPI_SR_TXE));
    mDev->DR = word;
    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
//    while (mDev->SR & SPI_SR_BSY); // wait while SPI is busy
    return mDev->DR;
}

void Spi::transfer(unsigned char* data,unsigned char size)
{
    if (!mUseDmaRx && !mUseDmaTx)
    {
        for(int i =0; i<size; i++)
        {
            mDev->DR = data[i];
            while (!(mDev->SR & 0x0001)); // wait for RX Not Empty
            data[i] =  mDev->DR;
        }
    }
    else
    {
        if (mUseDmaRx)
        {
            mDmaRx->setSingleBuffer(data, size);
            mDmaRx->start();
        }
        if (mUseDmaTx)
        {
            mDmaTx->setSingleBuffer(data, size);
            mDmaTx->start();
        }
    }
}

void Spi::transferWordAsync(unsigned short word)
{
    mDev->DR = word;
}
//---------------------------------------------------------------------------

void Spi::setUseDmaRx(bool useDma)
{
    if (isOpen())
        throw Exception::ResourceBusy;
    mUseDmaRx = useDma;
}

void Spi::setUseDmaTx(bool useDma)
{
    if (isOpen())
        throw Exception::ResourceBusy;
    mUseDmaTx = useDma;
}
//---------------------------------------------------------------------------

void Spi::setTransferCompleteEvent(SpiDataEvent e)
{
    onTransferComplete = e;
    enableInterrupt();
}
//---------------------------------------------------------------------------

void Spi::enableInterrupt()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = mIrq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    SPI_I2S_ITConfig(mDev, SPI_I2S_IT_RXNE, ENABLE);
}

void Spi::handleInterrupt()
{
    if (SPI_I2S_GetITStatus(mDev, SPI_I2S_IT_RXNE) == SET)
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
