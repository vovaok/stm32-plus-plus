#ifndef _RCC_H
#define _RCC_H

#include "stm32.h"
#include "gpio.h"

class Rcc
{
public:
    static Rcc *instance();

#if defined(STM32F4) || defined(STM32G4) || defined(STM32F3)
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
    
#if defined(STM32F4) || defined(STM32G4) || defined(STM32L4)
    constexpr uint32_t hsiValue() const {return 16000000;}
#elif defined(STM32F3)
    constexpr uint32_t hsiValue() const {return 8000000;}
#endif

    bool configPll(uint32_t hseValue, uint32_t sysClk);
    bool configPll(uint32_t sysClk);
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

#if defined(STM32L4) || defined (STM32G4)
//    #define APB1ENR                 APB1ENR1
//    #define RCC_APB1ENR_SPI2EN      RCC_APB1ENR1_SPI2EN
//    #define RCC_APB1ENR_SPI3EN      RCC_APB1ENR1_SPI3EN
//    #define RCC_APB1ENR_USART2EN    RCC_APB1ENR1_USART2EN
//    #define RCC_APB1ENR_USART3EN    RCC_APB1ENR1_USART3EN
//    #define RCC_APB1ENR_UART4EN     RCC_APB1ENR1_UART4EN
//    #define RCC_APB1ENR_UART5EN     RCC_APB1ENR1_UART5EN
//    #define RCC_APB1ENR_TIM2EN      RCC_APB1ENR1_TIM2EN
//    #define RCC_APB1ENR_TIM3EN      RCC_APB1ENR1_TIM3EN
//    #define RCC_APB1ENR_TIM4EN      RCC_APB1ENR1_TIM4EN
//    #define RCC_APB1ENR_TIM5EN      RCC_APB1ENR1_TIM5EN
//    #define RCC_APB1ENR_TIM6EN      RCC_APB1ENR1_TIM2EN
//    #define RCC_APB1ENR_TIM7EN      RCC_APB1ENR1_TIM7EN
#endif

#endif