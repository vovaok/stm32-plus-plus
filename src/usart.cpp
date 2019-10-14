#include "usart.h"

#if defined(STM32F37X) 
    Usart *Usart::mUsarts[3] = {0, 0, 0};
    #define SR ISR
    #define USART_SR_TC USART_ISR_TC
#else
    Usart *Usart::mUsarts[6] = {0, 0, 0, 0, 0, 0};
    #define RDR DR
    #define TDR DR
#endif
//---------------------------------------------------------------------------

Usart::Usart(UsartNo number, int baudrate, Config config, Gpio::Config pinTx, Gpio::Config pinRx) :
    mUseDmaRx(true), mUseDmaTx(true),
    mDmaRx(0L),
    mDmaTx(0L),
    mRxPos(0),
    mRxIrqDataCounter(0),
    mRxBufferSize(64),
    mTxPos(0),
    mTxReadPos(0),
    mTxBufferSize(64),
    mLineEnd("\n"),
    mHalfDuplex(false)
{  
    Gpio::config(pinRx);
    Gpio::config(pinTx);
    commonConstructor(number, baudrate, config);
}

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
    mLineEnd("\n"),
    mHalfDuplex(false)
{
    UsartNo number = UsartNone;
    if (pinTx != Gpio::NoConfig)
        number = getUsartByPin(pinTx);
    else if (pinRx != Gpio::NoConfig)
        number = getUsartByPin(pinRx);
    Gpio::config(pinRx);
    Gpio::config(pinTx);
    
    commonConstructor(number, 57600, Mode8N1);
    
    if (pinRx == Gpio::NoConfig) // if only TX pin is given
    {
        // half-duplex mode:
        mDev->CR3 |= USART_CR3_HDSEL;
        mHalfDuplex = true;
    }
}

void Usart::commonConstructor(UsartNo number, int baudrate, Config config)
{
    switch (number)
    {
      case Usart1:  
        mDev = USART1;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#if defined(STM32F37X)
        mDmaChannelRx = Dma::Channel5_USART1_RX;
        mDmaChannelTx = Dma::Channel4_USART1_TX;
#else
        mDmaChannelRx = Dma::ChannelUsart1_Rx;
        mDmaChannelTx = Dma::ChannelUsart1_Tx;
#endif
        mIrq = USART1_IRQn;
        break;
        
      case Usart2:  
        mDev = USART2;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
#if defined(STM32F37X)
        mDmaChannelRx = Dma::Channel6_USART2_RX;
        mDmaChannelTx = Dma::Channel7_USART2_TX;
#else        
        mDmaChannelRx = Dma::ChannelUsart2_Rx;
        mDmaChannelTx = Dma::ChannelUsart2_Tx;
#endif
        mIrq = USART2_IRQn;
        break;
        
      case Usart3:  
        mDev = USART3;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#if defined(STM32F37X)
        mDmaChannelRx = Dma::Channel3_USART3_RX;
        mDmaChannelTx = Dma::Channel2_USART3_TX;
#else        
        mDmaChannelRx = Dma::ChannelUsart3_Rx;
        mDmaChannelTx = Dma::ChannelUsart3_Tx;
#endif
        mIrq = USART3_IRQn;
        break;
        
#if !defined(STM32F37X)       
      case Usart4:  
        mDev = UART4;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
        mDmaChannelRx = Dma::ChannelUsart4_Rx;
        mDmaChannelTx = Dma::ChannelUsart4_Tx;
        mIrq = UART4_IRQn;
        break;
        
      case Usart5:  
        mDev = UART5;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
        mDmaChannelRx = Dma::ChannelUsart5_Rx;
        mDmaChannelTx = Dma::ChannelUsart5_Tx;
        mIrq = UART5_IRQn;
        break;
        
      case Usart6:  
        mDev = USART6;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
        mDmaChannelRx = Dma::ChannelUsart6_Rx;
        mDmaChannelTx = Dma::ChannelUsart6_Tx;
        mIrq = USART6_IRQn;
        break;
#endif
    }
    
    mUsarts[number-1] = this;
        
    mConfig.USART_BaudRate = baudrate;
    mConfig.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    mConfig.USART_Mode = 0;
    setConfig(config);
}

Usart::~Usart()
{
    if (mDmaRx)
        delete mDmaRx;
    if (mDmaTx)
        delete mDmaTx;
    USART_DeInit(mDev);
}
//---------------------------------------------------------------------------

UsartNo Usart::getUsartByPin(Gpio::Config pin)
{
    switch (pin)
    {
#if defined(STM32F37X)
        // USART1
        case Gpio::USART1_CK_PA8: case Gpio::USART1_TX_PA9: case Gpio::USART1_RX_PA10: case Gpio::USART1_CTS_PA11:
        case Gpio::USART1_RTS_PA12: case Gpio::USART1_TX_PB6: case Gpio::USART1_RX_PB7:
        case Gpio::USART1_TX_PC4: case Gpio::USART1_RX_PC5: case Gpio::USART1_TX_PE0: case Gpio::USART1_RX_PE1:
        return Usart1;
        // USART2
        case Gpio::USART2_CTS_PA0: case Gpio::USART2_RTS_PA1: case Gpio::USART2_TX_PA2: case Gpio::USART2_RX_PA3:
        case Gpio::USART2_CK_PA4: case Gpio::USART2_TX_PB3: case Gpio::USART2_RX_PB4: case Gpio::USART2_CK_PB5:
        case Gpio::USART2_CTS_PD3: case Gpio::USART2_RTS_PD4: case Gpio::USART2_TX_PD5: case Gpio::USART2_RX_PD6:
        case Gpio::USART2_CK_PD7: case Gpio::USART2_CK_PF7:
        return Usart2;
        // USART3
        case Gpio::USART3_CTS_PA13: case Gpio::USART3_TX_PB8: case Gpio::USART3_RX_PB9: case Gpio::USART3_TX_PB10: 
        case Gpio::USART3_RTS_PB14: case Gpio::USART3_TX_PC10: case Gpio::USART3_RX_PC11: case Gpio::USART3_CK_PC12:
        case Gpio::USART3_TX_PD8: case Gpio::USART3_RX_PD9: case Gpio::USART3_CK_PD10: case Gpio::USART3_CTS_PD11:
        case Gpio::USART3_RTS_PD12: case Gpio::USART3_RX_PE15: case Gpio::USART3_RTS_PF6:
        return Usart3;
#else        
        // USART1
        case Gpio::USART1_CK_PA8: case Gpio::USART1_TX_PA9: case Gpio::USART1_RX_PA10: case Gpio::USART1_CTS_PA11:
        case Gpio::USART1_RTS_PA12: case Gpio::USART1_TX_PB6: case Gpio::USART1_RX_PB7:
        return Usart1;
        // USART2
        case Gpio::USART2_CTS_PA0: case Gpio::USART2_RTS_PA1: case Gpio::USART2_TX_PA2: case Gpio::USART2_RX_PA3:
        case Gpio::USART2_CK_PA4: case Gpio::USART2_CTS_PD3: case Gpio::USART2_RTS_PD4: case Gpio::USART2_TX_PD5:
        case Gpio::USART2_RX_PD6: case Gpio::USART2_CK_PD7:
        return Usart2;
        // USART3
        case Gpio::USART3_TX_PB10: case Gpio::USART3_RX_PB11: case Gpio::USART3_CK_PB12: case Gpio::USART3_CTS_PB13:
        case Gpio::USART3_RTS_PB14: case Gpio::USART3_TX_PC10: case Gpio::USART3_RX_PC11: case Gpio::USART3_CK_PC12:
        case Gpio::USART3_TX_PD8: case Gpio::USART3_RX_PD9: case Gpio::USART3_CK_PD10: case Gpio::USART3_CTS_PD11:
        case Gpio::USART3_RTS_PD12:
        return Usart3;
        // UART4
        case Gpio::UART4_TX_PA0: case Gpio::UART4_RX_PA1: case Gpio::UART4_TX_PC10: case Gpio::UART4_RX_PC11:
        return Usart4;
        // UART5
        case Gpio::UART5_TX_PC12: case Gpio::UART5_RX_PD2:
        return Usart5;
        // USART6
        case Gpio::USART6_TX_PC6: case Gpio::USART6_RX_PC7: case Gpio::USART6_CK_PC8: case Gpio::USART6_CK_PG7:
        case Gpio::USART6_RTS_PG8: case Gpio::USART6_RX_PG9: case Gpio::USART6_RTS_PG12: case Gpio::USART6_CTS_PG13:
        case Gpio::USART6_TX_PG14: case Gpio::USART6_CTS_PG15:
        return Usart6;
#endif
        // no usart
        default:
        return UsartNone;
    }
}
//---------------------------------------------------------------------------

void Usart::init()
{
    uint32_t tmpreg = 0x00, apbclock = 0x00;
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;

    /*---------------------------- USART CR2 Configuration -----------------------*/
    tmpreg = mDev->CR2;
    tmpreg &= (uint32_t)~((uint32_t)USART_CR2_STOP); // Clear STOP[13:12] bits
    /* Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit :
      Set STOP[13:12] bits according to USART_StopBits value */
    tmpreg |= (uint32_t)mConfig.USART_StopBits;
    mDev->CR2 = (uint16_t)tmpreg; // Write to USART CR2

    /*---------------------------- USART CR1 Configuration -----------------------*/
    tmpreg = mDev->CR1;
    tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE)); // Clear M, PCE, PS, TE and RE bits
    /* Configure the USART Word Length, Parity and mode: 
     Set the M bits according to USART_WordLength value 
     Set PCE and PS bits according to USART_Parity value
     Set TE and RE bits according to USART_Mode value */
    tmpreg |= (uint32_t)mConfig.USART_WordLength | mConfig.USART_Parity | mConfig.USART_Mode;
    mDev->CR1 = (uint16_t)tmpreg; // Write to USART CR1

    /*---------------------------- USART CR3 Configuration -----------------------*/  
    tmpreg = mDev->CR3;
    tmpreg &= (uint32_t)~((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE)); // Clear CTSE and RTSE bits
    /* Configure the USART HFC : 
      Set CTSE and RTSE bits according to USART_HardwareFlowControl value */
    tmpreg |= mConfig.USART_HardwareFlowControl;
    mDev->CR3 = (uint16_t)tmpreg; // Write to USART CR3

    /*---------------------------- USART BRR Configuration -----------------------*/
    // Configure the USART Baud Rate
    #if defined(STM32F37X)
    apbclock = (mDev == USART1)? Rcc::pClk2(): Rcc::pClk1();
    #else
    apbclock = ((mDev == USART1) || (mDev == USART6))? Rcc::pClk2(): Rcc::pClk1();
    #endif

#if defined(STM32F37X)
    tmpreg = apbclock / mConfig.USART_BaudRate;
    
#else
    // Determine the integer part
    if ((mDev->CR1 & USART_CR1_OVER8) != 0)
    {
        // Integer part computing in case Oversampling mode is 8 Samples
        integerdivider = ((25 * apbclock) / (2 * (mConfig.USART_BaudRate)));    
    }
    else
    {
        // Integer part computing in case Oversampling mode is 16 Samples
        integerdivider = ((25 * apbclock) / (4 * (mConfig.USART_BaudRate)));    
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
    
    mDev->CR1 &= ~(USART_CR1_UE);
    // Write to USART BRR register
    mDev->BRR = (uint16_t)tmpreg;
    mDev->CR1 |= (USART_CR1_UE);
}
//---------------------------------------------------------------------------

bool Usart::open(OpenMode mode)
{
    if (isOpen())
        return false;
    
    mConfig.USART_Mode = 0;
    if (mode & Read)
    {
        mConfig.USART_Mode |= USART_Mode_Rx;
        mRxBuffer.resize(mRxBufferSize); ///////////////////////////////!!!!!! power of two only!!!!
        if (mUseDmaRx)
        {
            if (!mDmaRx)
            {
#if defined(STM32F37X)
                mDmaRx = new Dma(mDmaChannelRx);
#else
                mDmaRx = Dma::getStreamForPeriph(mDmaChannelRx);
#endif
            }
            mDmaRx->setCircularBuffer(mRxBuffer.data(), mRxBuffer.size());
            mDmaRx->setSource((void*)&mDev->RDR, 1);
        }
        else
        {
            NVIC_InitTypeDef NVIC_InitStructure;
            NVIC_InitStructure.NVIC_IRQChannel = mIrq;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            
            USART_ITConfig(mDev, USART_IT_RXNE, ENABLE);
        }
    }
    if (mode & Write)
    {
        mConfig.USART_Mode |= USART_Mode_Tx;
        mTxBuffer.resize(mTxBufferSize);
        if (mUseDmaTx)
        {
            if (!mDmaTx)
#if defined(STM32F37X)
                mDmaTx = new Dma(mDmaChannelTx);
#else              
                mDmaTx = Dma::getStreamForPeriph(mDmaChannelTx);
#endif
            mDmaTx->setSink((void*)&mDev->TDR, 1);
            mDmaTx->setTransferCompleteEvent(EVENT(&Usart::dmaTxComplete));
        }
    }
    
    init();
    
    if (mDmaRx && mUseDmaRx)
    {
        USART_DMACmd(mDev, USART_DMAReq_Rx, ENABLE);
        mDmaRx->start();
        (unsigned int&)mode |= Read;
    }
    if (mDmaTx && mUseDmaTx)
    {
        USART_DMACmd(mDev, USART_DMAReq_Tx, ENABLE);
        (unsigned int&)mode |= Write;
    }
    
    USART_Cmd(mDev, ENABLE);
    
    SerialInterface::open(mode);
    return true;
}

void Usart::close()
{
    if (mDmaRx)
    {
        USART_DMACmd(mDev, USART_DMAReq_Rx, DISABLE);
        mDmaRx->stop(true);
        delete mDmaRx;
        mDmaRx = 0L;
    }
    if (mDmaTx)
    {
        USART_DMACmd(mDev, USART_DMAReq_Tx, DISABLE);
        mDmaTx->stop(true);
        delete mDmaTx;
        mDmaTx = 0L;
    }
    
    USART_Cmd(mDev, DISABLE);
    SerialInterface::close();
}
//---------------------------------------------------------------------------

static int written = 0;

int Usart::write(const char *data, int size)
{  
    if (!mDmaTx)
    {
        for (int i=0; i<size; i++)
        {
            while (!(mDev->SR & USART_FLAG_TC));
            mDev->TDR = data[i];
        }
        while (!(mDev->SR & USART_FLAG_TC));
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
    
    if (mHalfDuplex)
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

int Usart::write(const ByteArray &ba)
{
    return write(ba.data(), ba.size());
}

int Usart::read(ByteArray &ba)
{
    int mask = mRxBuffer.size() - 1;
    int curPos = mDmaRx? (mRxBuffer.size() - mDmaRx->dataCounter()): mRxIrqDataCounter;
    int read = (curPos - mRxPos) & mask;
    if (m7bits)
    {
        for (int i=mRxPos; i<mRxPos+read; i++)
            ba.append(mRxBuffer[i & mask] & 0x7F);
    }
    else
    {
        for (int i=mRxPos; i<mRxPos+read; i++)
            ba.append(mRxBuffer[i & mask]);
    }
       
    mRxPos = curPos;
    return read;
}

bool Usart::canReadLine()
{
    int mask = mRxBuffer.size() - 1;
    int curPos = mDmaRx? (mRxBuffer.size() - mDmaRx->dataCounter()): mRxIrqDataCounter;
    int read = (curPos - mRxPos) & mask;
    unsigned char bitmask = m7bits? 0x7F: 0xFF;
    for (int i=mRxPos; i<mRxPos+read; i++)
    {
        bool flag = true;
        for (int j=0; j<mLineEnd.size(); j++)
        {
            if ((mRxBuffer[(i+j) & mask] & bitmask) != mLineEnd[j])
            {
                flag = false;
                break;
            }
        }
        if (flag)
            return true;
    }
    return false;
}

int Usart::readLine(ByteArray &ba)
{
    int mask = mRxBuffer.size() - 1;
    int curPos = mDmaRx? (mRxBuffer.size() - mDmaRx->dataCounter()): mRxIrqDataCounter;
    int read = (curPos - mRxPos) & mask;
    int i = mRxPos, endi = 0;
    unsigned char bitmask = m7bits? 0x7F: 0xFF;
    for (; (i<mRxPos+read) && (endi<mLineEnd.size()); i++)
    {
        char b = mRxBuffer[i & mask] & bitmask;
        ba.append(b);
        if (b == mLineEnd[endi])
            endi++;
        else
            endi = 0;
    }
       
    read = (i - mRxPos) & mask;
    mRxPos = i & mask;
    return read;
}
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
        if (mHalfDuplex)
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
    mConfig.USART_BaudRate = baudrate;
    init();
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
    
    mConfig.USART_Parity = config & 0x0600;
    mConfig.USART_StopBits = config & 0x3000;
    mConfig.USART_WordLength = (config << 8) & 0x1000;
    init();
}

void Usart::setBufferSize(int size_bytes)
{
    if (isOpen())
        throw Exception::resourceBusy;
    mRxBufferSize = upper_power_of_two(size_bytes);
    mTxBufferSize = mRxBufferSize;
}

void Usart::setUseDmaRx(bool useDma)
{
    if (isOpen())
        throw Exception::resourceBusy;
    mUseDmaRx = useDma;
}

void Usart::setUseDmaTx(bool useDma)
{
    if (isOpen())
        throw Exception::resourceBusy;
    mUseDmaTx = useDma;
}

void Usart::setLineEnd(ByteArray lineend)
{
    mLineEnd = lineend;
}
//---------------------------------------------------------------------------

unsigned char Usart::getErrorCode() const
{
    unsigned char err = mDev->SR & 0xF;
    unsigned char dummy;
    if (err)
        dummy = mDev->DR; // read from data register to clear error flag(s)
    dummy = dummy;
    return err;
}
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
  
    if (USART_GetITStatus(mDev, USART_IT_RXNE) == SET)
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