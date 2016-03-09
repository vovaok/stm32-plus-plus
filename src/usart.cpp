#include "usart.h"

Usart *Usart::mUsarts[6] = {0, 0, 0, 0, 0, 0};
//---------------------------------------------------------------------------

Usart::Usart(UsartNo number, int baudrate, Config config, Gpio::Config pinTx, Gpio::Config pinRx) :
    mUseDma(true),
    mDmaRx(0L),
    mDmaTx(0L),
    mRxPos(0),
    mRxIrqDataCounter(0),
    mRxBufferSize(64),
    mLineEnd("\n")
{  
    Gpio::config(pinRx);
    Gpio::config(pinTx);
                 
    switch (number)
    {
      case Usart1:  
        mDev = USART1;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        mDmaChannelRx = Dma::ChannelUsart1_Rx;
        mDmaChannelTx = Dma::ChannelUsart1_Tx;
        mIrq = USART1_IRQn;
        break;
        
      case Usart2:  
        mDev = USART2;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        mDmaChannelRx = Dma::ChannelUsart2_Rx;
        mDmaChannelTx = Dma::ChannelUsart2_Tx;
        mIrq = USART2_IRQn;
        break;
        
      case Usart3:  
        mDev = USART3;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        mDmaChannelRx = Dma::ChannelUsart3_Rx;
        mDmaChannelTx = Dma::ChannelUsart3_Tx;
        mIrq = USART3_IRQn;
        break;
        
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
    }
    
    mUsarts[number-1] = this;
    
    mConfig.USART_BaudRate = baudrate;
    mConfig.USART_Parity = config & 0x0600;
    mConfig.USART_StopBits = config & 0x3000;
    mConfig.USART_WordLength = (config << 8) & 0x1000;
    mConfig.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    mConfig.USART_Mode = 0;
    init();
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
    apbclock = ((mDev == USART1) || (mDev == USART6))? Rcc::pClk2(): Rcc::pClk1();

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

    // Write to USART BRR register
    mDev->BRR = (uint16_t)tmpreg;
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
        if (mUseDma)
        {
            if (!mDmaRx)
                mDmaRx = Dma::getStreamForPeriph(mDmaChannelRx);
            mDmaRx->setCircularBuffer(mRxBuffer.data(), mRxBuffer.size());
            mDmaRx->setSource((void*)&mDev->DR, 1);
        }
        else
        {
            NVIC_InitTypeDef NVIC_InitStructure;
            NVIC_InitStructure.NVIC_IRQChannel = mIrq;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            
            USART_ITConfig(mDev, USART_IT_RXNE, ENABLE);
        }
    }
    if (mode & Write)
    {
        mConfig.USART_Mode |= USART_Mode_Tx;
#warning USART: DMA is not used on Tx
//        if (!mDmaTx)
//            mDmaTx = Dma::getStreamForPeriph(mDmaChannelTx);
//        mDmaTx->setSink((void*)&mDev->DR, 1);
    }
    
    init();
    
    if (mDmaRx && mUseDma)
    {
        USART_DMACmd(mDev, USART_DMAReq_Rx, ENABLE);
        mDmaRx->start();
        (unsigned int&)mode |= Read;
    }
    if (mDmaTx && mUseDma)
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

int Usart::write(const ByteArray &ba)
{
    if (!mDmaTx)
    {
        for (int i=0; i<ba.size(); i++)
        {
            while (!(mDev->SR & USART_FLAG_TC));
            mDev->DR = ba[i];
        }
        while (!(mDev->SR & USART_FLAG_TC));
        return ba.size();
    }
    
    if (mDmaTx->dataCounter() > 0)
        return -1;
    mTxBuffer = ba;
    mDmaTx->stop(true);
    mDmaTx->setSingleBuffer(mTxBuffer.data(), mTxBuffer.size());
    mDmaTx->start();
    return 0;
}

int Usart::read(ByteArray &ba)
{
    int mask = mRxBuffer.size() - 1;
    int curPos = mRxBuffer.size() - (mDmaRx? mDmaRx->dataCounter(): mRxIrqDataCounter);
    int read = (curPos - mRxPos) & mask;
    for (int i=mRxPos; i<mRxPos+read; i++)
        ba.append(mRxBuffer[i & mask]);
       
    mRxPos = curPos;
    return read;
}

bool Usart::canReadLine()
{
    int mask = mRxBuffer.size() - 1;
    int curPos = mRxBuffer.size() - (mDmaRx? mDmaRx->dataCounter(): mRxIrqDataCounter);
    int read = (curPos - mRxPos) & mask;
    for (int i=mRxPos; i<mRxPos+read; i++)
    {
        bool flag = true;
        for (int j=0; j<mLineEnd.size(); j++)
        {
            if (mRxBuffer[(i+j) & mask] != mLineEnd[j])
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
    int curPos = mRxBuffer.size() - (mDmaRx? mDmaRx->dataCounter(): mRxIrqDataCounter);
    int read = (curPos - mRxPos) & mask;
    int i = mRxPos, endi = 0;
    for (; (i<mRxPos+read) && (endi<mLineEnd.size()); i++)
    {
        char b = mRxBuffer[i & mask];
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

void Usart::setBaudrate(int baudrate)
{
    mConfig.USART_BaudRate = baudrate;
    init();
}

void Usart::setConfig(Config config)
{
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
}

void Usart::setUseDma(bool useDma)
{
    if (isOpen())
        throw Exception::resourceBusy;
    mUseDma = useDma;
}

void Usart::setLineEnd(ByteArray lineend)
{
    mLineEnd = lineend;
}
//---------------------------------------------------------------------------

void Usart::handleInterrupt()
{
    if (USART_GetITStatus(mDev, USART_IT_RXNE) == SET)
    {
        mRxBuffer[mRxIrqDataCounter++] = mDev->DR & (uint16_t)0x01FF;
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
   
#ifdef __cplusplus
}
#endif