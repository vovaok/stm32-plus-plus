#ifndef _RCC_H
#define _RCC_H

#include "stm32.h"
#include "gpio.h"

class Rcc
{
public:
    static Rcc *instance();

#if defined(STM32F4) || defined(STM32G4) || defined(STM32F3) || defined(STM32F0) || defined(STM32F7)
    enum ClockSource
    {
        HSI = 0x0,
        HSE = 0x1,
        PLL = 0x2,
        SYSCLK = 0x80,
        LSI,
        LSE,
#if defined(RCC_CR_PLLI2SON)
        PLLI2S,
#endif
#if defined(RCC_CR_PLLSAION)
        PLLSAI
#endif
    };
#elif defined(STM32L4)
    enum ClockSource
    {
        MSI = 0x0,
        HSI = 0x1,
        HSE = 0x2,
        PLL = 0x3
    };
#endif
    
#if defined(STM32F4) || defined(STM32G4) || defined(STM32L4) || defined(STM32F7)
    constexpr uint32_t hsiValue() const {return 16000000;}
#elif defined(STM32F3) || defined(STM32F0)
    constexpr uint32_t hsiValue() const {return 8000000;}
#endif

    bool configPll(uint32_t hseValue, uint32_t sysClk);
    bool configPll(uint32_t sysClk);
    void configPll(ClockSource pll, int freqP, int freqQ, int freqR);
    uint32_t hseValue() const {return mHseValue;}
    uint32_t sysClk() const {return mSysClk;}
    uint32_t hClk() const {return mAHBClk;}
    uint32_t pClk1() const {return mAPB1Clk;}
    uint32_t pClk2() const {return mAPB2Clk;}

    bool setSystemClockSource(ClockSource src);
    ClockSource systemClockSource() const;
    bool setEnabled(ClockSource src, bool enabled);
    bool isReady(ClockSource src);

    void setPeriphEnabled(void *periphBase, bool enabled=true);
    void resetPeriph(void *periphBase);

    int getPeriphClk(void *periphBase);

#if defined(LTDC)
    int configLtdcClock(int frequency); //!< return real programmed frequency
#endif
    
#if defined(STM32F4) || defined(STM32F7)
    struct PllCfgr
    {
        uint32_t M: 6; // only for main PLL
        uint32_t N: 9;
        uint32_t : 1;
        uint32_t P: 2;
        uint32_t : 4;
        uint32_t src: 1; // only for main PLL
        uint32_t : 1;
        uint32_t Q: 4;
        uint32_t R: 3;
    } __attribute__(( packed, aligned(4) ));
#endif

    // ''  It is highly recommended to change this
    //     prescaler only after reset before enabling
    //     the external oscillators and the PLLs       ''
    //                                    (c) datasheet
    void configClockOutput(Gpio::Config mco, ClockSource clk, int prescaler=1);

private:
    Rcc();
    static Rcc *m_self;

    bool measureHseFreq();
    
    // returns base address of periph bus
    uint32_t periphBusBase(void *periph);
    // returns offset of periph base relative to bus base
    uint32_t periphBusOffset(void *periph);

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