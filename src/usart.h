#ifndef _USART_H
#define _USART_H

#include "gpio.h"
#include "dma.h"
#include "rcc.h"
#include "core/device.h"

//using namespace Serial;
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

class Usart : public Device
{  
public:
    typedef enum
    {
        // CR1 related config
        WordLength8 = 0x0000,
        WordLength9 = 0x1000,
        ParityNone  = 0x0000,
        ParityEven  = 0x0400,
        ParityOdd   = 0x0600,
        // CR2 related config
        StopBits0_5 = 0x1000 << 16,
        StopBits1   = 0x0000 << 16,
        StopBits1_5 = 0x3000 << 16,
        StopBits2   = 0x2000 << 16,
      
        Mode8N1     = WordLength8 | ParityNone | StopBits1,
        Mode8E1     = WordLength9 | ParityEven | StopBits1,
        Mode7E1     = WordLength8 | ParityEven | StopBits1,
    } Config;
  
private:
    static Usart *mUsarts[6];
    USART_TypeDef *mDev;
    int mBaudrate;
    
    Gpio *m_pinDE;
    
    IRQn_Type mIrq;
    
    bool mUseDmaRx, mUseDmaTx;
    ByteArray mRxBuffer;
    ByteArray mTxBuffer;
    Dma::Channel mDmaChannelRx;
    Dma::Channel mDmaChannelTx;
    Dma *mDmaRx;
    Dma *mDmaTx;
    
    int mRxPos;
    int mRxIrqDataCounter;
    int mRxBufferSize;
    int mTxPos;
    int mTxReadPos;
    int mTxBufferSize;
    
    bool m7bits;
    
    void commonConstructor(int number);
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
    
public:
    Usart(Gpio::Config pinTx, Gpio::Config pinRx);
    ~Usart();
    
    void configPinDE(Gpio::PinName pin);
    
    void setBufferSize(int size_bytes);
    void setUseDmaRx(bool useDma);
    void setUseDmaTx(bool useDma);
    
    bool open(OpenMode mode = ReadWrite);
    void close();
    
    int bytesAvailable() const;
    
    virtual bool canReadLine() const; // override
    
//    int readLine(ByteArray &ba);
    
    void setBaudrate(int baudrate);
    int baudrate() const {return mBaudrate;}
    void setConfig(Config config);
    
//    unsigned char getErrorCode() const;
    
    void setClockPin(Gpio::Config pinCk, bool inverted = false);
    
protected:
    virtual int writeData(const char *data, int size); // override;
    virtual int readData(char *data, int size); // override;
};

#endif