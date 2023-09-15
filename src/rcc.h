#ifndef _RCC_H
#define _RCC_H

#include "stm32.h"

#if defined(STM32L4)
#define APB1ENR APB1ENR1
#define RCC_APB1ENR_SPI2EN RCC_APB1ENR1_SPI2EN
#define RCC_APB1ENR_SPI3EN RCC_APB1ENR1_SPI3EN
#endif

class Rcc
{
public:    
    static Rcc *instance();
    
#if defined(STM32F4)
    enum ClockSource
    {
        HSI = 0x0,
        HSE = 0x1,
        PLL = 0x2
    };
#else
    enum ClockSource
    {
        MSI = 0x0,
        HSI = 0x1,
        HSE = 0x2,
        PLL = 0x3
    };
#endif
    
    bool configPll(uint32_t hseValue, uint32_t sysClk);
    bool configPll(uint32_t sysClk);
    constexpr uint32_t hsiValue() const {return 16000000;}
    uint32_t hseValue() const {return mHseValue;}
    uint32_t sysClk() const {return mSysClk;}
    uint32_t hClk() const {return mAHBClk;}
    uint32_t pClk1() const {return mAPB1Clk;}
    uint32_t pClk2() const {return mAPB2Clk;}
    
    bool setSystemClockSource(ClockSource src);
    ClockSource systemClockSource() const;
    bool setEnabled(ClockSource src, bool enabled);
    bool isReady(ClockSource src);
    
private:
    Rcc();
    static Rcc *m_self;
    
    bool measureHseFreq();
    
    uint32_t mHseValue;
    uint32_t mPllM;
    uint32_t mPllN;
#if !defined(STM32F37X)
    uint32_t mPllP;
    uint32_t mPllQ;
#endif
    uint32_t mSysClk;
    uint32_t mAHBClk;
    uint32_t mAPB1Clk;
    uint32_t mAPB2Clk;
};

extern Rcc &rcc();

#endif