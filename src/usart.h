#ifndef _USART_H
#define _USART_H

#include "gpio.h"
#include "dma.h"
#include "rcc.h"
#include "serial/serialinterface.h"

using namespace Serial;

typedef enum
{
    Usart1  = 1,
    Usart2  = 2,
    Usart3  = 3,
    Usart4  = 4,
    Usart5  = 5,
    Usart6  = 6,
} UsartNo;
//---------------------------------------------------------------------------

extern "C" void USART1_IRQHandler();
extern "C" void USART2_IRQHandler();
extern "C" void USART3_IRQHandler();
extern "C" void UART4_IRQHandler();
extern "C" void UART5_IRQHandler();
extern "C" void USART6_IRQHandler();
//---------------------------------------------------------------------------

class Usart : public SerialInterface
{  
public:
    typedef enum
    {
        ParityNone  = USART_Parity_No,
        ParityEven  = USART_Parity_Even,
        ParityOdd   = USART_Parity_Odd,
        StopBits0_5 = USART_StopBits_0_5,
        StopBits1   = USART_StopBits_1,
        StopBits1_5 = USART_StopBits_1_5,
        StopBits2   = USART_StopBits_2,
        WordLength8 = 0x0000,
        WordLength9 = 0x0010,
        Mode8N1     = WordLength8 | ParityNone | StopBits1,
        Mode8E1     = WordLength9 | ParityEven | StopBits1,
    } Config;
  
private:
    static Usart *mUsarts[6];
    USART_TypeDef *mDev;
    USART_InitTypeDef mConfig;
    bool mConfigured;
    bool mUseDma;
    ByteArray mRxBuffer;
    ByteArray mTxBuffer;
    Dma::DmaChannel mDmaChannelRx;
    Dma::DmaChannel mDmaChannelTx;
    Dma *mDmaRx;
    Dma *mDmaTx;
    IRQn mIrq;
    int mRxPos;
    int mRxIrqDataCounter;
    int mRxBufferSize;
    ByteArray mLineEnd;
    
    void init();
    
    friend void USART1_IRQHandler();
    friend void USART2_IRQHandler();
    friend void USART3_IRQHandler();
    friend void UART4_IRQHandler();
    friend void UART5_IRQHandler();
    friend void USART6_IRQHandler();
    
    void handleInterrupt();
    
public:
    Usart(UsartNo number, int baudrate, Config config, Gpio::Config pinTx, Gpio::Config pinRx);
    ~Usart();
    
    void setBufferSize(int size_bytes);
    void setUseDma(bool useDma);
    void setLineEnd(ByteArray lineend);
    
    bool open(OpenMode mode = ReadWrite);
    void close();
    
    int write(const ByteArray &ba);
    int read(ByteArray &ba);
    
    bool canReadLine();
    int readLine(ByteArray &ba);
    
    void setBaudrate(int baudrate);
    int baudrate() const {return mConfig.USART_BaudRate;}
    void setConfig(Config config);
};

#endif