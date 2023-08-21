#include "usart.h"

#if defined(STM32F37X) 
    #define SR ISR
    #define USART_SR_TC USART_ISR_TC
#else
    #define RDR DR
    #define TDR DR
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
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        mDmaChannelRx = Dma::USART1_RX_Stream2; // Dma::USART1_RX_Stream5
        mDmaChannelTx = Dma::USART1_TX_Stream7;
        mIrq = USART1_IRQn;
        break;
        
      case 2:  
        mDev = USART2;
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        mDmaChannelRx = Dma::USART2_RX_Stream5;
        mDmaChannelTx = Dma::USART2_TX_Stream6;
        mIrq = USART2_IRQn;
        break;
        
      case 3:  
        mDev = USART3;
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        mDmaChannelRx = Dma::USART3_RX_Stream1;
        mDmaChannelTx = Dma::USART3_TX_Stream3; // Dma::USART3_TX_Stream4
        mIrq = USART3_IRQn;
        break;
        
#if !defined(STM32F37X)       
      case 4:  
        mDev = UART4;
        RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
        mDmaChannelRx = Dma::UART4_RX_Stream2;
        mDmaChannelTx = Dma::UART4_TX_Stream4;
        mIrq = UART4_IRQn;
        break;
        
      case 5:  
        mDev = UART5;
        RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
        mDmaChannelRx = Dma::UART5_RX_Stream0;
        mDmaChannelTx = Dma::UART5_TX_Stream7;
        mIrq = UART5_IRQn;
        break;
        
      case 6:  
        mDev = USART6;
        RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
        mDmaChannelRx = Dma::USART6_RX_Stream1; //Dma::USART6_RX_Stream2
        mDmaChannelTx = Dma::USART6_TX_Stream6; //Dma::USART6_TX_Stream7
        mIrq = USART6_IRQn;
        break;
#endif
    }
    
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
    
    uint32_t cr1 = 0;
    if (mode & ReadOnly)
    {
        cr1 |= USART_CR1_RE;
        mRxBuffer.resize(mRxBufferSize); ///////////////////////////////!!!!!! power of two only!!!!
        if (mUseDmaRx)
        {
            if (!mDmaRx)
                mDmaRx = new Dma(mDmaChannelRx);
            mDmaRx->setCircularBuffer(mRxBuffer.data(), mRxBuffer.size());
            mDmaRx->setSource((void*)&mDev->RDR, 1);
        }
        if (!mUseDmaRx || onReadyRead)
        {
//            NVIC_InitTypeDef NVIC_InitStructure;
//            NVIC_InitStructure.NVIC_IRQChannel = mIrq;
//            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//            NVIC_Init(&NVIC_InitStructure);
          
            NVIC_SetPriority(mIrq, 1);
            NVIC_EnableIRQ(mIrq);
            mDev->CR1 |= USART_CR1_RXNEIE;
        }
    }
    if (mode & WriteOnly)
    {
        cr1 |= USART_CR1_TE;
        mTxBuffer.resize(mTxBufferSize);
        if (mUseDmaTx)
        {
            if (!mDmaTx)
                mDmaTx = new Dma(mDmaChannelTx);
            mDmaTx->setSink((void*)&mDev->TDR, 1);
            mDmaTx->setTransferCompleteEvent(EVENT(&Usart::dmaTxComplete));
        }
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
        mDev->CR3 &= ~USART_CR3_DMAR;
        mDmaRx->stop(true);
        delete mDmaRx;
        mDmaRx = 0L;
    }
    if (mDmaTx)
    {
        mDev->CR3 &= ~USART_CR3_DMAT;
        mDmaTx->stop(true);
        delete mDmaTx;
        mDmaTx = 0L;
    }
    
    // disable UART
    mDev->CR1 &= ~USART_CR1_UE;
    
    Device::close();
}
//---------------------------------------------------------------------------

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
    
    //  write through DMA
    int mask = mTxBuffer.size() - 1;
    int curPos = (mTxReadPos - mDmaTx->dataCounter()) & mask;
    int maxsize = (curPos - mTxPos - 1) & mask;
    int sz = size;
    if (sz > maxsize)
    {
        return -1;
//        sz = maxsize;
    }
    for (int i=0; i<sz; i++)
        mTxBuffer[(mTxPos + i) & mask] = data[i];
    mTxPos = (mTxPos + sz) & mask;
    
    if (mDmaTx->dataCounter() > 0) // if transmission in progress...
        return sz; // dma restarts in irq handler
    
    // otherwise start new dma transfer
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
    return size;
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
        if (m_halfDuplex)
        {
            while (!(mDev->SR & USART_SR_TC)); // wait for last byte is being written
            
            if (m_pinDE)
                m_pinDE->reset();
            mDev->CR1 |= USART_CR1_RE;
            
            if (onBytesWritten)
                onBytesWritten();
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
//    mBaudrate = baudrate;
    
    #if defined(STM32F37X)
    int apbclock = (mDev == USART1)? Rcc::pClk2(): Rcc::pClk1();
    #else
    int apbclock = ((mDev == USART1) || (mDev == USART6))? Rcc::pClk2(): Rcc::pClk1();
    #endif

    uint32_t tmpreg = apbclock / baudrate;
    mBaudrate = apbclock / tmpreg;
    
#if defined(STM32F37X)
//    mBaudrate = apbclock / tmpreg;
#else
    
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
#endif
    
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
  
    if (!mUseDmaRx)
    {
        // read from USART_SR register followed by a read from USART_DR
        if (mDev->SR & USART_SR_RXNE)
        {
            mRxBuffer[mRxIrqDataCounter++] = mDev->RDR & (uint16_t)0x01FF;
            if (mRxIrqDataCounter >= mRxBuffer.size())
                mRxIrqDataCounter = 0;
        }
    }
    
    if (onReadyRead)
        onReadyRead();
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