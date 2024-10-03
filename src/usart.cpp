#include "usart.h"

#if defined(STM32F4) || defined(STM32F7)
#define USART1_RX_DMA   Dma::USART1_RX_Stream2; // Dma::USART1_RX_Stream5
#define USART1_TX_DMA   Dma::USART1_TX_Stream7;
#define USART2_RX_DMA   Dma::USART2_RX_Stream5;
#define USART2_TX_DMA   Dma::USART2_TX_Stream6;
#define USART3_RX_DMA   Dma::USART3_RX_Stream1;
#define USART3_TX_DMA   Dma::USART3_TX_Stream3; // Dma::USART3_TX_Stream4
#define UART4_RX_DMA    Dma::UART4_RX_Stream2;
#define UART4_TX_DMA    Dma::UART4_TX_Stream4;
#define UART5_RX_DMA    Dma::UART5_RX_Stream0;
#define UART5_TX_DMA    Dma::UART5_TX_Stream7;
#define USART6_RX_DMA   Dma::USART6_RX_Stream1; //Dma::USART6_RX_Stream2
#define USART6_TX_DMA   Dma::USART6_TX_Stream6; //Dma::USART6_TX_Stream7

#elif defined(STM32L4)
#define USART1_RX_DMA   Dma::USART1_RX_Channel5; // USART1_RX_Channel7
#define USART1_TX_DMA   Dma::USART1_TX_Channel4; // USART1_TX_Channel6
#define USART2_RX_DMA   Dma::USART2_RX_Channel6;
#define USART2_TX_DMA   Dma::USART2_TX_Channel7;
#define USART3_RX_DMA   Dma::USART3_RX_Channel3;
#define USART3_TX_DMA   Dma::USART3_TX_Channel2;
#define UART4_RX_DMA    Dma::UART4_RX_Channel5;
#define UART4_TX_DMA    Dma::UART4_TX_Channel3;
#define UART5_RX_DMA    Dma::UART5_RX_Channel2;
#define UART5_TX_DMA    Dma::UART5_TX_Channel1;

#elif defined (STM32G4)
#define USART1_RX_DMA   Dma::USART1_RX;
#define USART1_TX_DMA   Dma::USART1_TX;
#define USART2_RX_DMA   Dma::USART2_RX;
#define USART2_TX_DMA   Dma::USART2_TX;
#define USART3_RX_DMA   Dma::USART3_RX;
#define USART3_TX_DMA   Dma::USART3_TX;
#define UART4_RX_DMA    Dma::UART4_RX;
#define UART4_TX_DMA    Dma::UART4_TX;
#define UART5_RX_DMA    Dma::UART5_RX;
#define UART5_TX_DMA    Dma::UART5_TX;

#elif defined (STM32F3)
#define USART1_RX_DMA   Dma::Channel5_USART1_RX;
#define USART1_TX_DMA   Dma::Channel4_USART1_TX;
#define USART2_RX_DMA   Dma::Channel6_USART2_RX;
#define USART2_TX_DMA   Dma::Channel7_USART2_TX;
#define USART3_RX_DMA   Dma::Channel3_USART3_RX;
#define USART3_TX_DMA   Dma::Channel2_USART3_TX;

#endif

#if defined(STM32F4)
    #define RDR             DR
    #define TDR             DR
#else
    #define SR                      ISR
    #define USART_SR_TC             USART_ISR_TC
    #define USART_SR_RXNE           USART_ISR_RXNE
#endif

Usart *Usart::mUsarts[6] = {0L, 0L, 0L, 0L, 0L, 0L};
//---------------------------------------------------------------------------

Usart::Usart(Gpio::Config pinTx, Gpio::Config pinRx) :
    m_pinDE(0L),
    mUseDmaRx(true), mUseDmaTx(true),
    mDmaRx(0L),
    mDmaTx(0L),
    mRxPos(0),
    mRxIrqDataCounter(0),
    mRxBufferSize(64),
    mTxPos(0),
    mTxReadPos(0),
    mTxBufferSize(64)
{
    Device::m_sequential = true;

    int no = 0;
    if (pinTx != Gpio::NoConfig)
        no = GpioConfigGetPeriphNumber(pinTx);
    else if (pinRx != Gpio::NoConfig)
        no = GpioConfigGetPeriphNumber(pinRx);
    Gpio::config(pinRx);
    Gpio::config(pinTx);

    commonConstructor(no);

    // if only TX pin is given switch to half-duplex mode
    if (pinRx == Gpio::NoConfig)
    {
        m_halfDuplex = true;
        mDev->CR3 |= USART_CR3_HDSEL;
    }
}

void Usart::commonConstructor(int number)
{
//    const USART_TypeDef *devs[6] = {USART1, USART2, USART3, UART4, UART5, USART6};
//    const uint32_t rcc[6] = {};
//    const IRQn_Type irq[6] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};

    switch (number)
    {
      case 1:
        mDev = USART1;
        mDmaChannelRx = USART1_RX_DMA;
        mDmaChannelTx = USART1_TX_DMA;
        mIrq = USART1_IRQn;
        break;

      case 2:
        mDev = USART2;
        mDmaChannelRx = USART2_RX_DMA;
        mDmaChannelTx = USART2_TX_DMA;
        mIrq = USART2_IRQn;
        break;

      case 3:
        mDev = USART3;
        mDmaChannelRx = USART3_RX_DMA;
        mDmaChannelTx = USART3_TX_DMA;
        mIrq = USART3_IRQn;
        break;

#if defined(STM32F4) || defined(STM32L4) || defined(STM32G4) || defined(STM32F7)
      case 4:
        mDev = UART4;
        mDmaChannelRx = UART4_RX_DMA;
        mDmaChannelTx = UART4_TX_DMA;
        mIrq = UART4_IRQn;
        break;

      case 5:
        mDev = UART5;
        mDmaChannelRx = UART5_RX_DMA;
        mDmaChannelTx = UART5_TX_DMA;
        mIrq = UART5_IRQn;
        break;

#endif
#if defined(STM32F4)
      case 6:
        mDev = USART6;
        mDmaChannelRx = USART6_RX_DMA;
        mDmaChannelTx = USART6_TX_DMA;
        mIrq = USART6_IRQn;
        break;
#endif
    }

#if defined(STM32F3)
    switch(number)
    {
    case 1: RCC->APB2ENR |= RCC_APB2ENR_USART1EN; break;
    case 2: RCC->APB1ENR |= RCC_APB1ENR_USART2EN; break;
    case 3: RCC->APB1ENR |= RCC_APB1ENR_USART3EN; break;
    }
#else
    rcc().setPeriphEnabled(mDev);
#endif
    mUsarts[number - 1] = this;

    setConfig(Mode8N1);
    setBaudrate(57600);
}

Usart::~Usart()
{
    if (mDmaRx)
        delete mDmaRx;
    if (mDmaTx)
        delete mDmaTx;
    mDev->CR1 = 0;
    mDev->CR2 = 0;
    mDev->CR3 = 0;
}
//---------------------------------------------------------------------------

void Usart::setClockPin(Gpio::Config pinCk, bool inverted)
{
    Gpio::config(pinCk);
/// @todo inverted clk pin is not implemented
}

void Usart::configPinDE(Gpio::PinName pin)
{
    m_pinDE = new Gpio(pin, Gpio::Output);
    m_halfDuplex = true;
}
//---------------------------------------------------------------------------

bool Usart::open(OpenMode mode)
{
    if (isOpen())
        return false;

    bool enableIrq = false;
    uint32_t cr1 = 0;
    if (mode & ReadOnly)
    {
        cr1 |= USART_CR1_RE;
        mRxBuffer.resize(mRxBufferSize); ///////////////////////////////!!!!!! power of two only!!!!
        if (mUseDmaRx)
        {
            mDmaRx = Dma::instance(mDmaChannelRx);
//            if (!mDmaRx)
//                mDmaRx = new Dma(mDmaChannelRx);
            mDmaRx->setCircularBuffer(mRxBuffer.data(), mRxBuffer.size());
            mDmaRx->setSource((void*)&mDev->RDR, 1);
        }
        if (!mUseDmaRx || onReadyRead || m_characterMatch)
        {
            cr1 |= USART_CR1_RXNEIE;
            enableIrq = true;
        }
    }
    if (mode & WriteOnly)
    {
        cr1 |= USART_CR1_TE;
        mTxBuffer.resize(mTxBufferSize);
        if (mUseDmaTx)
        {
            mDmaTx = Dma::instance(mDmaChannelTx);
//            if (!mDmaTx)
//                mDmaTx = new Dma(mDmaChannelTx);
            mDmaTx->setSink((void*)&mDev->TDR, 1);
            mDmaTx->setTransferCompleteEvent(EVENT(&Usart::dmaTxComplete));
            enableIrq = true; // TCIE interrupt will be enabled later on transfer complete event
        }
    }

    // enable interrupt if necessary
    if (enableIrq)
    {
        NVIC_SetPriority(mIrq, 1);
        NVIC_EnableIRQ(mIrq);
    }

    // enable receiver and transmitter if necessary
    mDev->CR1 = mDev->CR1 & ~(USART_CR1_RE | USART_CR1_TE) | cr1;

//    init();

    if (mDmaRx && mUseDmaRx)
    {
        mDev->CR3 |= USART_CR3_DMAR;
        mDmaRx->start();
        (unsigned int&)mode |= ReadOnly;
    }
    if (mDmaTx && mUseDmaTx)
    {
        mDev->CR3 |= USART_CR3_DMAT;
        (unsigned int&)mode |= WriteOnly;
    }

    // enable UART
    mDev->CR1 |= USART_CR1_UE;

    Device::open(mode);
    return true;
}

void Usart::close()
{
    if (mDmaRx)
    {
        mDmaRx->stop(true);
        mDev->CR3 &= ~USART_CR3_DMAR;
//        delete mDmaRx;
//        mDmaRx = 0L;
    }
    if (mDmaTx)
    {
        mDmaTx->stop(true);
        mDev->CR3 &= ~USART_CR3_DMAT;
//        delete mDmaTx;
//        mDmaTx = 0L;
    }

    // disable UART
    mDev->CR1 &= ~USART_CR1_UE;

    Device::close();
}
//---------------------------------------------------------------------------

//bool Usart::fillBuffer(const char *data, int size)
//{
//    char *dst = mTxBuffer.data() + mTxPos;
//    char *end = mTxBuffer.data() + mTxBuffer.size();
//    while (size--)
//    {
//        *dst++ = *data++;
//        if (dst >= end)
//            dst = mTxBuffer.data();
//    }
//    mTxPos = dst - mTxBuffer.data();
//    return true;
//}

int Usart::writeBuffer(const char *data, int size)
{
    if (size <= 0)
        return 0;

    int mask = mTxBuffer.size() - 1;
//    int curPos = (mTxReadPos - mDmaTx->dataCounter()) & mask;
//    int maxsize = (curPos - mTxPos - 1) & mask;
    int maxsize = (mTxReadPos - mDmaTx->dataCounter() - mTxPos - 1) & mask;
    if (size > maxsize) /// @todo: maybe write a bit of something...
    {
        return -1;
//        sz = maxsize;
    }

    char *begin = mTxBuffer.data();
    char *dst = begin + mTxPos;
    char *end = begin + mTxBuffer.size();
    int sz = size;
    while (sz--)
    {
        *dst++ = *data++;
        if (dst >= end)
            dst = begin;
    }
    mTxPos = dst - begin;
    return size;
}

int Usart::availableWriteCount() const
{
    int mask = mTxBuffer.size() - 1;
    return (mTxReadPos - mDmaTx->dataCounter() - mTxPos - 1) & mask;
}

int Usart::bytesAvailable() const
{
    int mask = mRxBuffer.size() - 1;
    if (mDmaRx)
        return (-mDmaRx->dataCounter() - mRxPos) & mask;
    else
        return (mRxIrqDataCounter - mRxPos) & mask;
}

int Usart::writeData(const char *data, int size)
{
    if (!mDmaTx)
    {
        if (m_halfDuplex)
        {
            mDev->CR1 &= ~USART_CR1_RE;
            if (m_pinDE)
                m_pinDE->set();
        }

        for (int i=0; i<size; i++)
        {
            while (!(mDev->SR & USART_SR_TC));
            mDev->TDR = data[i];
        }
        while (!(mDev->SR & USART_SR_TC));

        if (m_halfDuplex)
        {
            if (m_pinDE)
                m_pinDE->reset();
            mDev->CR1 |= USART_CR1_RE;
        }

        return size;
    }

    int sz = writeBuffer(data, size);
    if (sz < 0)
        return -1;
    if(sz ==0)
        return 0;

//    sz = (mTxPos - curPos) & mask;

//    if (mDmaTx->dataCounter() > 0) // if transmission in progress...
    if (mDmaTx->isEnabled() || !(mDev->SR & USART_SR_TC))
        return sz; // dma restarts in irq handler

    // otherwise start new dma transfer
    int mask = mTxBuffer.size() - 1;
    mDmaTx->stop(true);
    if (sz > mTxBufferSize - mTxReadPos)
        sz = mTxBufferSize - mTxReadPos;
    mDmaTx->setSingleBuffer(mTxBuffer.data() + mTxReadPos, sz);
    mTxReadPos = (mTxReadPos + sz) & mask;

    if (m_halfDuplex)
    {
        mDev->CR1 &= ~USART_CR1_RE;
        if (m_pinDE)
            m_pinDE->set();
    }

//    if (mHalfDuplex)
//        mDev->CR1 |= USART_CR1_RWU;

//    if (mHalfDuplex)
//    {
//        // wait for line idle:
//        while (!(mDev->SR & USART_SR_IDLE));
//    }

    mDmaTx->start();
    return sz;
}

int Usart::readData(char *data, int size)
{
    int cnt = bytesAvailable();
    int mask = mRxBuffer.size() - 1;

    if (cnt > size)
        cnt = size;
    if (m7bits)
    {
        for (int i=mRxPos; i<mRxPos+cnt; i++)
            *data++ = (mRxBuffer[i & mask] & 0x7F);
    }
    else
    {
        for (int i=mRxPos; i<mRxPos+cnt; i++)
            *data++ = (mRxBuffer[i & mask]);
    }

    mRxPos = (mRxPos + cnt) & mask;
    return cnt;
}

bool Usart::canReadLine() const
{
    int mask = mRxBuffer.size() - 1;
    int curPos = mDmaRx? (mRxBuffer.size() - mDmaRx->dataCounter()): mRxIrqDataCounter;
    int read = (curPos - mRxPos) & mask;
    for (int i=mRxPos; i<mRxPos+read; i++)
    {
        if ((mRxBuffer[i & mask] & 0x7F) == '\n')
            return true;
    }
    return false;
}

//int Usart::readLine(ByteArray &ba)
//{
//    int mask = mRxBuffer.size() - 1;
//    int curPos = mDmaRx? (mRxBuffer.size() - mDmaRx->dataCounter()): mRxIrqDataCounter;
//    int read = (curPos - mRxPos) & mask;
//    int i = mRxPos, endi = 0;
//    unsigned char bitmask = m7bits? 0x7F: 0xFF;
//    for (; (i<mRxPos+read) && (endi<mLineEnd.size()); i++)
//    {
//        char b = mRxBuffer[i & mask] & bitmask;
//        ba.append(b);
//        if (b == mLineEnd[endi])
//            endi++;
//        else
//            endi = 0;
//    }
//
//    read = (i - mRxPos) & mask;
//    mRxPos = i & mask;
//    return read;
//}
//---------------------------------------------------------------------------

void Usart::dmaTxComplete()
{
    int mask = mTxBuffer.size() - 1;
    int sz = (mTxPos - mTxReadPos) & mask;
    if (sz > mTxBufferSize - mTxReadPos)
        sz = mTxBufferSize - mTxReadPos;
    if (!sz)
    {
        mDmaTx->stop();
        // if there is necessary to handle the transfer completion:
        if (m_halfDuplex || onBytesWritten)
        {
            // wait for last byte is being written in the TX complete interrupt
            mDev->CR1 |= USART_CR1_TCIE;
        }
        return;
    }
    mDmaTx->setSingleBuffer(mTxBuffer.data() + mTxReadPos, sz);
    mTxReadPos = (mTxReadPos + sz) & mask;
    mDmaTx->start();
}
//---------------------------------------------------------------------------

void Usart::setBaudrate(int baudrate)
{
    int apbclock = rcc().getPeriphClk(mDev);

    uint32_t tmpreg = apbclock / baudrate;
    mBaudrate = apbclock / tmpreg;

    if (tmpreg < 8)
        THROW(Exception::OutOfRange);
    if (tmpreg < 16)
    {
        mDev->CR1 |= USART_CR1_OVER8;
        tmpreg = ((tmpreg & ~0x7) << 1) | (tmpreg & 0x7);
    }
    else
    {
        mDev->CR1 &= ~USART_CR1_OVER8;
    }

    mDev->BRR = (uint16_t)tmpreg;
}

void Usart::setConfig(Config config)
{
    switch (config)
    {
      case Mode7E1:
        m7bits = true;
        break;
      default:
        m7bits = false;
    }

    mDev->CR2 = mDev->CR2 & ~((uint32_t)USART_CR2_STOP) | (config >> 16);
    mDev->CR1 = mDev->CR1 & ~((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS)) | (config & 0xFFFF);
    mDev->CR3 = mDev->CR3 & ~((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE));
}
//---------------------------------------------------------------------------

void Usart::setBufferSize(int size_bytes)
{
    if (isOpen())
        THROW(Exception::ResourceBusy);
    mRxBufferSize = upper_power_of_two(size_bytes);
    mTxBufferSize = mRxBufferSize;
}

void Usart::setUseDmaRx(bool useDma)
{
    if (isOpen())
        THROW(Exception::ResourceBusy);
    mUseDmaRx = useDma;
}

void Usart::setUseDmaTx(bool useDma)
{
    if (isOpen())
        THROW(Exception::ResourceBusy);
    mUseDmaTx = useDma;
}

void Usart::setCharacterMatchEvent(char c, NotifyEvent e)
{
    m_characterMatch = c;
    m_characterMatchEvent = e;
}
//---------------------------------------------------------------------------

//unsigned char Usart::getErrorCode() const
//{
//    unsigned char err = mDev->SR & 0xF;
//    unsigned char dummy;
//    if (err)
//        dummy = mDev->DR; // read from data register to clear error flag(s)
//    dummy = dummy;
//    return err;
//}
//---------------------------------------------------------------------------

void Usart::handleInterrupt()
{
#if defined (STM32F37X)
    if (mDev->SR & USART_ISR_ORE)
    {
        // overrun
        mDev->ICR = USART_ICR_ORECF;
    }
#endif

    uint32_t sr = mDev->SR;

    bool rxenabled = (mDev->CR1 & USART_CR1_RE);

    if (sr & USART_SR_TC)
    {
        mDev->CR1 &= ~USART_CR1_TCIE;
        if (mDmaTx && m_halfDuplex)
        {
            if (m_pinDE)
                m_pinDE->reset();
            mDev->CR1 |= USART_CR1_RE;

            if (onBytesWritten)
                onBytesWritten();
        }
    }

    // read from USART_SR register followed by a read from USART_DR.
    // if DMA is used for RX, the flag is not set
    if (sr & USART_SR_RXNE)
    {
        mRxBuffer[mRxIrqDataCounter++] = mDev->RDR & (uint16_t)0x01FF;
        if (mRxIrqDataCounter >= mRxBuffer.size())
            mRxIrqDataCounter = 0;
    }

    if (rxenabled)
    {
        if (m_characterMatch)
        {
            if (mDev->RDR == m_characterMatch)
            {
    //            GPIOA->BSRR = 1; // this is for performance tests only!
                if (m_characterMatchEvent)
                    m_characterMatchEvent();
                else if (onReadyRead)
                    onReadyRead();
    //            GPIOA->BSRR = 1 << 16;
            }
        }
        else if (onReadyRead)
            onReadyRead();
    }
}
//---------------------------------------------------------------------------

#ifdef __cplusplus
 extern "C" {
#endif

void USART1_IRQHandler()
{
    if (Usart::mUsarts[0])
        Usart::mUsarts[0]->handleInterrupt();
}

void USART2_IRQHandler()
{
    if (Usart::mUsarts[1])
        Usart::mUsarts[1]->handleInterrupt();
}

void USART3_IRQHandler()
{
    if (Usart::mUsarts[2])
        Usart::mUsarts[2]->handleInterrupt();
}

#if !defined(STM32F37X)
void UART4_IRQHandler()
{
    if (Usart::mUsarts[3])
        Usart::mUsarts[3]->handleInterrupt();
}

void UART5_IRQHandler()
{
    if (Usart::mUsarts[4])
        Usart::mUsarts[4]->handleInterrupt();
}

void USART6_IRQHandler()
{
    if (Usart::mUsarts[5])
        Usart::mUsarts[5]->handleInterrupt();
}
#endif

#ifdef __cplusplus
}
#endif