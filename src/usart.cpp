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
    mUseDmaRx(true), mUseDmaTx(true),
    mDmaRx(0L),
    mDmaTx(0L),
    mRxPos(0),
    mRxIrqDataCounter(0),
    mRxBufferSize(64),
    mTxPos(0),
    mTxReadPos(0),
    mTxBufferSize(64),
    mLineEnd("\n")
{
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
        else
        {
//            NVIC_InitTypeDef NVIC_InitStructure;
//            NVIC_InitStructure.NVIC_IRQChannel = mIrq;
//            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//            NVIC_Init(&NVIC_InitStructure);
          
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

static int written = 0;

int Usart::writeData(const char *data, int size)
{  
    if (!mDmaTx)
    {
        for (int i=0; i<size; i++)
        {
            while (!(mDev->SR & USART_SR_TC));
            mDev->TDR = data[i];
        }
        while (!(mDev->SR & USART_SR_TC));
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
    written += sz;
    
    if (mDmaTx->dataCounter() > 0) // if transmission in progress...
        return sz; // dma restarts in irq handler
    
    // otherwise start new dma transfer
    mDmaTx->stop(true);
    if (sz > mTxBufferSize - mTxReadPos)
        sz = mTxBufferSize - mTxReadPos;
    mDmaTx->setSingleBuffer(mTxBuffer.data() + mTxReadPos, sz);
    mTxReadPos = (mTxReadPos + sz) & mask;
    
    if (m_halfDuplex)
        mDev->CR1 &= ~USART_CR1_RE;
    
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
    int mask = mRxBuffer.size() - 1;
    int curPos = mDmaRx? (mRxBuffer.size() - mDmaRx->dataCounter()): mRxIrqDataCounter;
    int read = (curPos - mRxPos) & mask;
    if (read > size)
        read = size;
    if (m7bits)
    {
        for (int i=mRxPos; i<mRxPos+read; i++)
            *data++ = (mRxBuffer[i & mask] & 0x7F);
    }
    else
    {
        for (int i=mRxPos; i<mRxPos+read; i++)
            *data++ = (mRxBuffer[i & mask]);
    }
       
    mRxPos = curPos;
    return read;
}

//bool Usart::canReadLine()
//{
//    int mask = mRxBuffer.size() - 1;
//    int curPos = mDmaRx? (mRxBuffer.size() - mDmaRx->dataCounter()): mRxIrqDataCounter;
//    int read = (curPos - mRxPos) & mask;
//    unsigned char bitmask = m7bits? 0x7F: 0xFF;
//    for (int i=mRxPos; i<mRxPos+read; i++)
//    {
//        bool flag = true;
//        for (int j=0; j<mLineEnd.size(); j++)
//        {
//            if ((mRxBuffer[(i+j) & mask] & bitmask) != mLineEnd[j])
//            {
//                flag = false;
//                break;
//            }
//        }
//        if (flag)
//            return true;
//    }
//    return false;
//}
//
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
            mDev->CR1 |= USART_CR1_RE;
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
    mBaudrate = baudrate;
    
    #if defined(STM32F37X)
    int apbclock = (mDev == USART1)? Rcc::pClk2(): Rcc::pClk1();
    #else
    int apbclock = ((mDev == USART1) || (mDev == USART6))? Rcc::pClk2(): Rcc::pClk1();
    #endif

    uint32_t tmpreg = 0;
    
#if defined(STM32F37X)
    tmpreg = apbclock / mConfig.USART_BaudRate;
    
#else
    uint32_t integerdivider = 0;
    uint32_t fractionaldivider = 0x00;
    // Determine the integer part
    if (mDev->CR1 & USART_CR1_OVER8)
    {
        // Integer part computing in case Oversampling mode is 8 Samples
        integerdivider = (25 * apbclock) / (2 * mBaudrate);    
    }
    else
    {
        // Integer part computing in case Oversampling mode is 16 Samples
        integerdivider = (25 * apbclock) / (4 * mBaudrate);    
    }
    tmpreg = (integerdivider / 100) << 4;

    // Determine the fractional part
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    // Implement the fractional part in the register
    if ((mDev->CR1 & USART_CR1_OVER8) != 0)
        tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
    else
        tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
#endif
    
    // Write to USART BRR register
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
    
    mConfig = config;
    
    mDev->CR2 = mDev->CR2 & ~((uint32_t)USART_CR2_STOP) | (mConfig >> 16);
    mDev->CR1 = mDev->CR1 & ~((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | (mConfig & 0xFFFF)));
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

void Usart::setLineEnd(ByteArray lineend)
{
    mLineEnd = lineend;
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
  
    // read from USART_SR register followed by a read from USART_DR
    if (mDev->SR & USART_SR_RXNE)
    {
        mRxBuffer[mRxIrqDataCounter++] = mDev->RDR & (uint16_t)0x01FF;
        if (mRxIrqDataCounter >= mRxBuffer.size())
            mRxIrqDataCounter = 0;
        //USART_ClearITPendingBit(mDev, USART_IT_RXNE);
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