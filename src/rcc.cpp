#include "rcc.h"

__NO_INIT( unsigned long Rcc::mHseValue );
__NO_INIT( unsigned long Rcc::mPllM );
__NO_INIT( unsigned long Rcc::mPllN );
__NO_INIT( unsigned long Rcc::mPllP );
__NO_INIT( unsigned long Rcc::mPllQ );
__NO_INIT( unsigned long Rcc::mSysClk );
__NO_INIT( unsigned long Rcc::mAHBClk );
__NO_INIT( unsigned long Rcc::mAPB1Clk );
__NO_INIT( unsigned long Rcc::mAPB2Clk );

void Rcc::configPll(unsigned long hseValue, unsigned long sysClk)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    
    mHseValue = 0;
    
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
        if (hseValue == 0)
        {
            // measure HSE frequency
            RCC->APB2ENR |= (1<<18); // enable peripheral clock for TIM11
            unsigned long rccCfgr = RCC->CFGR;
            unsigned long temp = rccCfgr;
            temp &= ~(0x1F << 16);
            temp |= (30 << 16); // RTC = HSE / 30
            RCC->CFGR = temp;
            
            int loopCount = 10;
            int fr0, fr1;
            TIM_TypeDef *tim11 = TIM11;
            asm("ADD  R1, %[tim], #16\n"        // R1 = TIM1->SR
                "ADD  R2, %[tim], #52\n"        // R2 = TIM1->CCR1
                "MOV  R3, %[cnt]\n"             // counter = 10
                "MOV  R0, #2\n"
                "STRH R0, [%[tim], #80]\n"      // TIM11->OR = 0x0002; // HSE connect to TIM11_CH1 input
                "MOVW R0, #65535\n"
                "STR  R0, [%[tim], #44]\n"      // TIM11->ARR = 0xFFFF;
                "MOV  R0, #1\n"
                "STRH R0, [%[tim], #24]\n"      // TIM11->CCMR1 = 0x0001; // CC1 channel configured as input
                "STRH R0, [%[tim], #32]\n"      // TIM11->CCER = 0x0001; // enable capture of rising edge
                "STRH R0, [%[tim]]\n"           // TIM11->CR1 = 0x0001; // enable TIM11
                "MOV  R0, #0\n"
                "STRH R0, [R1]\n"               // TIM11->SR = 0;
                "wait0:\n"
                "LDR R0, [R1]\n"
                "LSLS R0, R0, #30\n"            // test bit 2
                "BPL wait0\n"                   // wait for bit 2
                "LDR %[f0], [R2]\n"             // fr0 = TIM11->CCR1;
                "wait1: LDR R0, [R1]\n"
                "LSLS R0, R0, #30\n"            // test bit 2
                "BPL wait1\n"                   // wait for bit 2
                "LDR %[f1], [R2]\n"             // fr1 = TIM11->CCR1;
                "SUBS R3, R3, #1\n"             // decrement counter
                "BNE wait1\n"                   // continue loop
                "MOVS R0, #0\n"
                "STRH R0, [%[tim]]\n"           // TIM11->CR1 = 0x0000; // disable TIM11
                "STRH R0, [R1]\n"               // TIM11->SR = 0;
                : [f0]"=r"(fr0), [f1]"=r"(fr1)
                : [tim]"r"(tim11), [cnt]"r"(loopCount)
                : "cc", "R0", "R1", "R2", "R3");     
            
            RCC->CFGR = rccCfgr;
            RCC->APB2ENR &= ~(1<<18); // disable peripheral clock to TIM11
            // end of measure
            
            // calculate hseValue
            fr1 -= fr0;
            hseValue = ((16 * 30) * loopCount + fr1 / 2) / fr1;
//#if defined(STM32F429_439xx)
//#warning RCC FUCKING HACK!!! Tak ne doljno bit!!
//            hseValue = (hseValue + 2);// / 4;      
//#endif
            hseValue *= 1000000;
        }
        
        // calc pllM, pllN, etc...
        unsigned long pllvco = sysClk << 1;
        if (hseValue > 26000000)
            hseValue = 16000000;
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
        //end of calc
        
      
        /* Select regulator voltage output Scale 1 mode */
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
        PWR->CR |= PWR_CR_VOS;

        /* HCLK = SYSCLK / 1*/
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
        mAHBClk = mSysClk >> 0;

    #if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx)      
        /* PCLK2 = HCLK / 2*/
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
        mAPB2Clk = mAHBClk >> 1;
        
        /* PCLK1 = HCLK / 4*/
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
        mAPB1Clk = mAHBClk >> 2;
    #endif /* STM32F40_41xxx || STM32F427_437x || STM32F429_439xx */

    #if defined (STM32F401xx)
        /* PCLK2 = HCLK / 2*/
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
        mAPB2Clk = mAHBClk;
        
        /* PCLK1 = HCLK / 4*/
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
        mAPB1Clk = mAHBClk >> 1;
    #endif /* STM32F401xx */
       
        /* Configure the main PLL */
        RCC->PLLCFGR = mPllM | (mPllN << 6) | (((mPllP >> 1) -1) << 16) |
                       (RCC_PLLCFGR_PLLSRC_HSE) | (mPllQ << 24);

        /* Enable the main PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till the main PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }
       
    #if defined (STM32F427_437xx) || defined (STM32F429_439xx)
        /* Enable the Over-drive to extend the clock frequency to 180 Mhz */
        PWR->CR |= PWR_CR_ODEN;
        while((PWR->CSR & PWR_CSR_ODRDY) == 0)
        {
        }
        PWR->CR |= PWR_CR_ODSWEN;
        while((PWR->CSR & PWR_CSR_ODSWRDY) == 0)
        {
        }      
        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
    #endif /* STM32F427_437x || STM32F429_439xx  */

    #if defined (STM32F40_41xxx)     
        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
    #endif /* STM32F40_41xxx  */

    #if defined (STM32F401xx)
        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
    #endif /* STM32F401xx */

        /* Select the main PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= RCC_CFGR_SW_PLL;

        /* Wait till the main PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
        {
        }
      
//        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // HCLK = SYSCLK / 1
//        mAHBClk = mSysClk >> 0;
//        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; // PCLK2 = HCLK / 2
//        mAPB2Clk = mAHBClk >> 1;
//        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4; // PCLK1 = HCLK / 4
//        mAPB1Clk = mAHBClk >> 2;
//        
//        // Configure the main PLL
//        RCC->PLLCFGR = mPllM | (mPllN << 6) | (((mPllP >> 1) -1) << 16) |
//                       (RCC_PLLCFGR_PLLSRC_HSE) | (mPllQ << 24);
//
//        // Enable the main PLL 
//        RCC->CR |= RCC_CR_PLLON;
//        // Wait till the main PLL is ready 
//        while((RCC->CR & RCC_CR_PLLRDY) == 0);
//
//        // Configure Flash prefetch, Instruction cache, Data cache and wait state 
//        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
//
//        // Select the main PLL as system clock source 
//        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
//        RCC->CFGR |= RCC_CFGR_SW_PLL;
//
//        // Wait till the main PLL is used as system clock source 
//        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    }
    else
    { 
        // If HSE fails to start-up, the application will have wrong clockconfiguration.
        throw Exception::badSoBad;
    }
}
