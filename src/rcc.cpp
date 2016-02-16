#include "rcc.h"

__no_init unsigned long Rcc::mHseValue;
__no_init unsigned long Rcc::mPllM;
__no_init unsigned long Rcc::mPllN;
__no_init unsigned long Rcc::mPllP;
__no_init unsigned long Rcc::mPllQ;
__no_init unsigned long Rcc::mSysClk;
__no_init unsigned long Rcc::mAHBClk;
__no_init unsigned long Rcc::mAPB1Clk;
__no_init unsigned long Rcc::mAPB2Clk;

void Rcc::configPll(unsigned long hseValue, unsigned long sysClk)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
 
    // calc pllM, pllN, etc...
    unsigned long pllvco = sysClk << 1;
    mHseValue = hseValue;
    mPllM = mHseValue / 1000000;
    if (mPllM > 0x3F)
        throw Exception::badSoBad;
    mPllN = pllvco / 1000000;
    if (mPllN > 0x1FF)
        throw Exception::badSoBad;
    mPllP = 2;
    mPllQ = pllvco / 48000000;
    
    mSysClk = mHseValue / mPllM * mPllN / mPllP;
    
    // Enable HSE
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
    // Wait till HSE is ready and if Time out is reached exit;
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
        HSEStatus = (uint32_t)0x01;
    else
        HSEStatus = (uint32_t)0x00;

    if (HSEStatus == (uint32_t)0x01)
    {
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // HCLK = SYSCLK / 1
        mAHBClk = mSysClk >> 0;
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; // PCLK2 = HCLK / 2
        mAPB2Clk = mAHBClk >> 1;
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4; // PCLK1 = HCLK / 4
        mAPB1Clk = mAHBClk >> 2;
        
        // Configure the main PLL
        RCC->PLLCFGR = mPllM | (mPllN << 6) | (((mPllP >> 1) -1) << 16) |
                       (RCC_PLLCFGR_PLLSRC_HSE) | (mPllQ << 24);

        // Enable the main PLL 
        RCC->CR |= RCC_CR_PLLON;
        // Wait till the main PLL is ready 
        while((RCC->CR & RCC_CR_PLLRDY) == 0);

        // Configure Flash prefetch, Instruction cache, Data cache and wait state 
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

        // Select the main PLL as system clock source 
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= RCC_CFGR_SW_PLL;

        // Wait till the main PLL is used as system clock source 
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    }
    else
    { 
        // If HSE fails to start-up, the application will have wrong clockconfiguration.
        throw Exception::badSoBad;
    }
}