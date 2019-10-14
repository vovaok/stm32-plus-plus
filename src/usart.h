#ifndef _USART_H
#define _USART_H

#include "gpio.h"
#include "dma.h"
#include "rcc.h"
#include "serial/serialinterface.h"

using namespace Serial;

typedef enum
{
    UsartNone = 0,
    Usart1    = 1,
    Usart2    = 2,
    Usart3    = 3,
#if !defined(STM32F37X)
    Usart4    = 4,
    Usart5    = 5,
    Usart6    = 6,
#endif
} UsartNo;
//---------------------------------------------------------------------------

extern "C" void USART1_IRQHandler();
extern "C" void USART2_IRQHandler();
extern "C" void USART3_IRQHandler();
#if !defined(STM32F37X)
extern "C" void UART4_IRQHandler();
extern "C" void UART5_IRQHandler();
extern "C" void USART6_IRQHandler();
#endif
//---------------------------------------------------------------------------

class Usart : public SerialInterface
{  
public:
    typedef enum
    {
        ParityNone  = USART_Parity_No,
        ParityEven  = USART_Parity_Even,
        ParityOdd   = USART_Parity_Odd,
#if !defined(STM32F37X)
        StopBits0_5 = USART_StopBits_0_5,
#endif
        StopBits1   = USART_StopBits_1,
        StopBits1_5 = USART_StopBits_1_5,
        StopBits2   = USART_StopBits_2,
        WordLength8 = 0x0000,
        WordLength9 = 0x0010,
        Mode8N1     = WordLength8 | ParityNone | StopBits1,
        Mode8E1     = WordLength9 | ParityEven | StopBits1,
        Mode7E1     = WordLength8 | ParityEven | StopBits1,
    } Config;
  
private:
    #if defined(STM32F37X) 
    static Usart *mUsarts[3];
    #else
    static Usart *mUsarts[6];
    #endif
    USART_TypeDef *mDev;
    USART_InitTypeDef mConfig;
    bool mConfigured;
    bool mUseDmaRx, mUseDmaTx;
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
    int mTxPos;
    int mTxReadPos;
    int mTxBufferSize;
    ByteArray mLineEnd;
    bool m7bits;
    bool mHalfDuplex;
    
    void commonConstructor(UsartNo number, int baudrate, Config config);
    void init();
       
    void dmaTxComplete();
    
    friend void USART1_IRQHandler();
    friend void USART2_IRQHandler();
    friend void USART3_IRQHandler();
    #if !defined(STM32F37X)
    friend void UART4_IRQHandler();
    friend void UART5_IRQHandler();
    friend void USART6_IRQHandler();
    #endif
    
    void handleInterrupt();
    
    static UsartNo getUsartByPin(Gpio::Config pin);
    
public:
    Usart(UsartNo number, int baudrate, Config config, Gpio::Config pinTx, Gpio::Config pinRx);
    Usart(Gpio::Config pinTx, Gpio::Config pinRx);
    ~Usart();
    
    void setBufferSize(int size_bytes);
    void setUseDmaRx(bool useDma);
    void setUseDmaTx(bool useDma);
    
    void setLineEnd(ByteArray lineend);
    const ByteArray &lineEnd() const {return mLineEnd;}
    
    bool open(OpenMode mode = ReadWrite);
    void close();
    
    int write(const char *data, int size);
    int write(const ByteArray &ba);
    int read(ByteArray &ba);
    
    bool canReadLine();
    int readLine(ByteArray &ba);
    
    void setBaudrate(int baudrate);
    int baudrate() const {return mConfig.USART_BaudRate;}
    void setConfig(Config config);
    
    bool isHalfDuplex() const {return mHalfDuplex;}
    
    unsigned char getErrorCode() const;
};

#endif