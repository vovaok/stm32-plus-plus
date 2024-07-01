#include "spi.h"

#if defined(STM32F4)
#define SPI1_DMA_CHANNEL_RX     Dma::SPI1_RX_Stream2; // Dma::SPI1_RX_Stream0;
#define SPI1_DMA_CHANNEL_TX     Dma::SPI1_TX_Stream3; // Dma::SPI1_TX_Stream5;
#define SPI2_DMA_CHANNEL_RX     Dma::SPI2_RX_Stream3;
#define SPI2_DMA_CHANNEL_TX     Dma::SPI2_TX_Stream4;
#define SPI3_DMA_CHANNEL_RX     Dma::SPI3_RX_Stream0; // Dma::SPI3_RX_Stream2;
#define SPI3_DMA_CHANNEL_TX     Dma::SPI3_TX_Stream7; // DMa::SPI3_TX_Stream5;
#define SPI4_DMA_CHANNEL_RX     Dma::SPI4_RX_Stream0; // Dma::SPI4_RX_Stream3;
#define SPI4_DMA_CHANNEL_TX     Dma::SPI4_TX_Stream1; // Dma::SPI4_TX_Stream4;
#define SPI5_DMA_CHANNEL_RX     Dma::SPI5_RX_Stream5; // Dma::SPI5_RX_Stream3;
#define SPI5_DMA_CHANNEL_TX     Dma::SPI5_TX_Stream4; // Dma::SPI5_TX_Stream6;
#define SPI6_DMA_CHANNEL_RX     Dma::SPI6_TX_Stream5; // CONFLICT with SPI5_DMA_CHANNEL_RX!
#define SPI6_DMA_CHANNEL_TX     Dma::SPI6_RX_Stream6;

#elif defined(STM32L4)
#define SPI1_DMA_CHANNEL_RX     Dma::SPI1_RX_Channel2; // Dma::SPI1_RX_Channel3;
#define SPI1_DMA_CHANNEL_TX     Dma::SPI1_TX_Channel3; // Dma::SPI1_TX_Channel4;
#define SPI2_DMA_CHANNEL_RX     Dma::SPI2_RX_Channel4;
#define SPI2_DMA_CHANNEL_TX     Dma::SPI2_TX_Channel5;
#define SPI3_DMA_CHANNEL_RX     Dma::SPI3_RX_Channel1;
#define SPI3_DMA_CHANNEL_TX     Dma::SPI3_TX_Channel2;

#elif defined(STM32G4)
#define SPI1_DMA_CHANNEL_RX     Dma::SPI1_RX;
#define SPI1_DMA_CHANNEL_TX     Dma::SPI1_TX;
#define SPI2_DMA_CHANNEL_RX     Dma::SPI2_RX;
#define SPI2_DMA_CHANNEL_TX     Dma::SPI2_TX;
#define SPI3_DMA_CHANNEL_RX     Dma::SPI3_RX;
#define SPI3_DMA_CHANNEL_TX     Dma::SPI3_TX;
#define SPI4_DMA_CHANNEL_RX     Dma::SPI4_RX;
#define SPI4_DMA_CHANNEL_TX     Dma::SPI4_TX;

#elif defined(STM32F303x8)
#define SPI1_DMA_CHANNEL_RX     Dma::Channel2_SPI1_RX;
#define SPI1_DMA_CHANNEL_TX     Dma::Channel3_SPI1_TX;
#endif

Spi *Spi::mSpies[6] = {0L, 0L, 0L, 0L, 0L, 0L};

Spi::Spi(Gpio::Config sck, Gpio::Config miso, Gpio::Config mosi) :
    mDev(0L),
    mUseDmaRx(false), mUseDmaTx(false),
    mDmaRx(0L),
    mDmaTx(0L)
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
        mIrq = SPI1_IRQn;
        mDmaChannelRx = SPI1_DMA_CHANNEL_RX;
        mDmaChannelTx = SPI1_DMA_CHANNEL_TX;
        break;
#if defined(SPI2)
      case 2:
        mDev = SPI2;
        mIrq = SPI2_IRQn;
        mDmaChannelRx = SPI2_DMA_CHANNEL_RX;
        mDmaChannelTx = SPI2_DMA_CHANNEL_TX;
        break;
#endif
#if defined(SPI3)
      case 3:
        mDev = SPI3;
        mIrq = SPI3_IRQn;
        mDmaChannelRx = SPI3_DMA_CHANNEL_RX;
        mDmaChannelTx = SPI3_DMA_CHANNEL_TX;
        break;
#endif
#if defined(SPI4)
      case 4:
        mDev = SPI4;
        mIrq = SPI4_IRQn;
        mDmaChannelRx = SPI4_DMA_CHANNEL_RX;
        mDmaChannelTx = SPI4_DMA_CHANNEL_TX;
        break;
#endif
#if defined(SPI5)
      case 5:
        mDev = SPI5;
        mIrq = SPI5_IRQn;
        mDmaChannelRx = SPI5_DMA_CHANNEL_RX;
        mDmaChannelTx = SPI5_DMA_CHANNEL_TX;
        break;
#endif
#if defined(SPI6)
      case 6:
        mDev = SPI6;
        mIrq = SPI6_IRQn;
        mDmaChannelRx = SPI6_DMA_CHANNEL_RX;
        mDmaChannelTx = SPI6_DMA_CHANNEL_TX;
        break;
#endif
    }

    if (!mDev)
        THROW(Exception::InvalidPeriph);
    
    rcc().setPeriphEnabled(mDev);

    mConfig.SSI = 1;
    mConfig.SSM = 1;
    setDataSize(8); // by default
}
//---------------------------------------------------------------------------

void Spi::setConfig(Config cfg)
{
    bool en = mConfig.enable;
    cfg.enable = 0;
    mConfig = cfg;
    mConfig.SSI = 1;
    mConfig.SSM = 1;
    mDev->CR1 = mConfig.cr1;
    mDev->CR2 = mConfig.cr2;
    if (en)
    {
        mConfig.enable = 1;
        mDev->CR1 = mConfig.cr1;
    }
}

void Spi::updateConfig()
{
    mDev->CR2 = mConfig.cr2;
    mDev->CR1 = mConfig.cr1;
}

void Spi::setMasterMode()
{
    mConfig.master = 1;
    updateConfig();
}

void Spi::setDataSize(int size)
{
#if defined(SPI_FIFO_IMPL) && SPI_FIFO_IMPL == 1
    size = BOUND(4, size, 16);
    mConfig.FRXTH = size <= 8? 1: 0;
    mConfig.DS = (size - 1) & 15;
#else
    mConfig.DFF = (size == 16)? 1: 0;
#endif
    m_dataSize = size;
    updateConfig();
}

void Spi::setCPOL_CPHA(bool CPOL, bool CPHA)
{
    mConfig.CPOL = CPOL? 1: 0;
    mConfig.CPHA = CPHA? 1: 0;
    updateConfig();
}

void Spi::setBaudratePrescaler(int psc)
{
    mConfig.baudrate = psc & 7;
    updateConfig();
}

void Spi::setBaudrate(int value)
{
    int pclk = rcc().getPeriphClk(mDev);
    int psc = log2i(pclk / value) - 1;
    if (psc < 0)
        psc = 0;
    setBaudratePrescaler(psc);
}

int Spi::baudrate() const
{
    int pclk = rcc().getPeriphClk(mDev);
    return pclk >> (mConfig.baudrate + 1);
}
//--------------------------------------------------------------------------

void Spi::open()
{
    if (isOpen())
        return;// false;

    if (mUseDmaRx)
    {
        if (!mDmaRx)
        mDmaRx = new Dma(mDmaChannelRx);
        int dataSize = (m_dataSize > 8)? 2: 1;
//        size /= dataSize;
        mDmaRx->setSource(&mDev->DR, dataSize);
//        if (circular)
//            mDmaRx->setCircularBuffer(data, size);
//        else
//            mDmaRx->setSingleBuffer(data, size);
        mConfig.RXDMAEN = 1;
    }
    
//    if (mDmaRx)
//        mDmaRx->start();

    if (mUseDmaTx)
    {
        if (!mDmaTx)
            mDmaTx = new Dma(mDmaChannelTx);
        if (onBytesWritten)
            mDmaTx->setTransferCompleteEvent(EVENT(&Spi::handleDmaInterrupt));
        if (m_dataSize <= 8)
            mDmaTx->setSink((void*)&mDev->DR, 1);
        else
            mDmaTx->setSink((void*)&mDev->DR, 2);
        mConfig.TXDMAEN = 1;
    }

    updateConfig();
    
    mConfig.enable = 1;
    mDev->CR1 = mConfig.cr1;
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
    if (mDmaTx)
    {
        mConfig.TXDMAEN = 0;
        updateConfig();
        mDmaTx->stop(true);
        delete mDmaTx;
        mDmaTx = 0L;
    }

    mConfig.enable = 0;
    mDev->CR1 = mConfig.cr1;
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
    while (size--)
    {
        *((__IO uint8_t *)(&mDev->DR)) = *data;
        while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
        *data++ = *((__IO uint8_t *)(&mDev->DR));
    }
}

bool Spi::transferDma(const uint8_t *data, uint8_t *buffer, int size)
{
    if (mDmaTx->isEnabled() && !mDmaTx->isComplete())
        return false;
    
    if (m_dataSize <= 8)
    {
        mDmaRx->setSingleBuffer(buffer, size);
        mDmaTx->setSingleBuffer(const_cast<uint8_t*>(data), size);
    }
    else
    {
        mDmaRx->setSingleBuffer(buffer, size / 2);
        mDmaTx->setSingleBuffer(const_cast<uint8_t*>(data), size / 2);
    }
    if (buffer)
        mDmaRx->start();
    mDmaTx->start();
    
//    while (!mDmaRx->isComplete());
    
    return true;
}

void Spi::transfer(const uint8_t *data, uint8_t *buffer, int size)
{
    while (size--)
    {
        *((__IO uint8_t *)(&mDev->DR)) = *data++;
        while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
        *buffer++ = *((__IO uint8_t *)(&mDev->DR));
    }
}

uint8_t Spi::read()
{
    *((__IO uint8_t *)(&mDev->DR)) = 0x00;
    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
    return *((__IO uint8_t *)(&mDev->DR));
}

uint8_t Spi::write(uint8_t word)
{
    *((__IO uint8_t *)(&mDev->DR)) = word;
    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
    return *((__IO uint8_t *)(&mDev->DR));
}

uint16_t Spi::read16()
{
    *((__IO uint16_t *)(&mDev->DR)) = 0x00;
    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
    return *((__IO uint16_t *)(&mDev->DR));
}

uint16_t Spi::write16(uint16_t word)
{
    *((__IO uint16_t *)(&mDev->DR)) = word;
    while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
    return *((__IO uint16_t *)(&mDev->DR));
}

bool Spi::read(uint8_t* data, int size)
{
    if (mDmaRx && mDmaTx)
    {
        if (mDmaRx->isEnabled() && !mDmaRx->isComplete())
            return false;
        if (m_dataSize <= 8)
        {
            mDmaRx->setSingleBuffer(const_cast<uint8_t*>(data), size);
            mDmaTx->setSingleBuffer(const_cast<uint8_t*>(data), size);
        }
        else
        {
            mDmaRx->setSingleBuffer(const_cast<uint8_t*>(data), size / 2);
            mDmaTx->setSingleBuffer(const_cast<uint8_t*>(data), size / 2);
        }
        mDmaRx->start();
        mDmaTx->start();
    }
    else if (m_dataSize <= 8)
    {
        while (size--)
        {
            *((__IO uint8_t *)(&mDev->DR)) = 0x00;
            while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
            *data++ = *((__IO uint8_t *)(&mDev->DR));
        }
    }
    else
    {
        size = (size + 1) >> 1;
        uint16_t *dst = reinterpret_cast<uint16_t *>(data);
        while (size--)
        {
             *((__IO uint16_t *)(&mDev->DR)) = 0x00;
            while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
            *dst++ = *((__IO uint16_t *)(&mDev->DR));
        }
    }
    return true;
}

bool Spi::write(const uint8_t *data, int size)
{
    if (mDmaTx)
    {
        if (mDmaTx->isEnabled() && !mDmaTx->isComplete())
            return false;
        if (m_dataSize <= 8)
            mDmaTx->setSingleBuffer(const_cast<uint8_t*>(data), size);
        else
            mDmaTx->setSingleBuffer(const_cast<uint8_t*>(data), size / 2);
        mDmaTx->start();
    }
    else
    {
        if (m_dataSize <= 8)
        {
            while (size--)
            {
                *((__IO uint8_t *)(&mDev->DR)) = *data++;
                while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
                (void)mDev->DR;
            }
        }
        else
        {
            size = (size + 1) >> 1;
            const uint16_t *src = reinterpret_cast<const uint16_t *>(data);
            while (size--)
            {
                *((__IO uint16_t *)(&mDev->DR)) = *src++;
                while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
                (void)mDev->DR;
            }
        }
    }
    return true;
}

void Spi::setRxBuffer(uint8_t *data, int size, bool circular)
{
    if (mDmaRx)
        mDmaRx->stop(true);
    else
        mDmaRx = new Dma(mDmaChannelRx);
    int dataSize = (m_dataSize > 8)? 2: 1;
    size /= dataSize;
    mDmaRx->setSource(&mDev->DR, dataSize);
    if (circular)
        mDmaRx->setCircularBuffer(data, size);
    else
        mDmaRx->setSingleBuffer(data, size);
    mConfig.RXDMAEN = 1;
    updateConfig();
    mDmaRx->start();
}

bool Spi::writeFill16(uint16_t *value, int count)
{
    if (mDmaTx)
    {
        if (mDmaTx->isEnabled() && !mDmaTx->isComplete())
            return false;
        mDmaTx->setMemorySource(value);
        mDmaTx->start(count);
    }
    else
    {
        uint16_t v = *value;
        while (count--)
        {
            *((__IO uint16_t *)(&mDev->DR)) = v;
            while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
            (void)mDev->DR;
        }
    }
    return true;
}

bool Spi::writeFill16(uint16_t *pattern, int patternSize, int count)
{
    if (mDmaTx)
    {
        if (mDmaTx->isEnabled() && !mDmaTx->isComplete())
            return false;
        mDmaTx->setCircularBuffer(pattern, patternSize);
        mDmaTx->start(count);
    }
    else
    {
        const uint16_t *begin = reinterpret_cast<const uint16_t *>(pattern);
        const uint16_t *end = reinterpret_cast<const uint16_t *>(pattern) + patternSize;
        const uint16_t *src = begin;
        while (count--)
        {
            *((__IO uint16_t *)(&mDev->DR)) = *src++;
            if (src >= end)
                src = begin;
            while (!(mDev->SR & SPI_SR_RXNE)); // wait for RX Not Empty
            (void)mDev->DR;
        }
    }
    return true;
}

void Spi::waitForBytesWritten()
{
    if (mDmaTx && mDmaTx->isEnabled())
    {
        while (!mDmaTx->isComplete());
        while (!(mDev->SR & SPI_SR_TXE));
        while (mDev->SR & SPI_SR_BSY);
        (void)mDev->DR; // read data register to make it empty
    }
}
//---------------------------------------------------------------------------

void Spi::setUseDmaRx(bool useDma)
{
    if (isOpen())
        THROW(Exception::ResourceBusy);
    mUseDmaRx = useDma;
}

void Spi::setUseDmaTx(bool useDma)
{
    if (isOpen())
        THROW(Exception::ResourceBusy);
    mUseDmaTx = useDma;
}
//---------------------------------------------------------------------------

void Spi::setTransferCompleteEvent(SpiDataEvent e)
{
    onTransferComplete = e;
    enableInterrupt();
}

void Spi::setTransferCompleteEvent(NotifyEvent e)
{
    onTxEnd = e;
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
    if (onTransferComplete)
        mConfig.RXNEIE = 1;
    if (onTxEnd)
        mConfig.TXEIE = 1;
    updateConfig();
}

void Spi::handleInterrupt()
{
    if ((mDev->SR & SPI_SR_RXNE) || (mDmaRx && (mDev->CR2 & SPI_CR2_RXNEIE)))
    {
        if (onTransferComplete)
            onTransferComplete(mDev->DR);
    }
    else if ((mDev->SR & SPI_SR_TXE) || (mDmaTx && (mDev->CR2 & SPI_CR2_TXEIE)))
    {
        if (onTxEnd)
            onTxEnd();
    }
}

void Spi::handleDmaInterrupt()
{
    if (mDmaTx)
    {
        mDmaTx->stop();
        // clear RX FIFO
        while (mDev->SR & SPI_SR_RXNE)
            (void)mDev->DR;
        if (onBytesWritten)
            onBytesWritten();
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
