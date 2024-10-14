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
    
    Usart(Gpio::Config pinTx, Gpio::Config pinRx);
    virtual ~Usart();
    
    void configPinDE(Gpio::PinName pin);
    
    void setBufferSize(int size_bytes);
    void setUseDmaRx(bool useDma);
    void setUseDmaTx(bool useDma);
    
    bool open(OpenMode mode = ReadWrite) override;
    void close();
    
    int bytesAvailable() const;
    
    virtual bool canReadLine() const; // override
    
    void setBaudrate(int baudrate);
    int baudrate() const {return mBaudrate;}
    void setConfig(Config config);
    
//    unsigned char getErrorCode() const;
    
    void setClockPin(Gpio::Config pinCk, bool inverted = false);
    
    void setCharacterMatchEvent(char c, NotifyEvent e = NotifyEvent()); // must be called before open()
  
protected:
    virtual int writeData(const char *data, int size); // override;
    virtual int readData(char *data, int size); // override;
    
    ByteArray mRxBuffer;
    int mRxPos;
    int mRxIrqDataCounter;
    int mRxBufferSize;
    
    ByteArray mTxBuffer;
    int mTxPos;
    int mTxReadPos;
    int mTxBufferSize;
    
//    virtual bool fillBuffer(const char *data, int size);
    
    int writeBuffer(const char *data, int size); // fill TX buffer without transmission
    int availableWriteCount() const;
    
private:
    static Usart *mUsarts[6];
    USART_TypeDef *mDev;
    IRQn_Type mIrq;
    int mBaudrate;
    char m_characterMatch = 0;
    NotifyEvent m_characterMatchEvent;    
    
    Gpio *m_pinDE;
    
    bool mUseDmaRx, mUseDmaTx;
    Dma::Channel mDmaChannelRx;
    Dma::Channel mDmaChannelTx;
    Dma *mDmaRx;
    Dma *mDmaTx;
    
    bool m7bits;
    
    void commonConstructor(int number);
    void init();
       
    void dmaTxComplete();
    void handleInterrupt();
    
    friend void USART1_IRQHandler();
    friend void USART2_IRQHandler();
    friend void USART3_IRQHandler();
    #if !defined(STM32F37X)
    friend void UART4_IRQHandler();
    friend void UART5_IRQHandler();
    friend void USART6_IRQHandler();
    #endif
};

#endif