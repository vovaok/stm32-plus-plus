#include "rcc.h"

__NO_INIT( unsigned long Rcc::mHseValue );
__NO_INIT( unsigned long Rcc::mPllM );
__NO_INIT( unsigned long Rcc::mPllN );
#if !defined(STM32F37X)
__NO_INIT( unsigned long Rcc::mPllP );
__NO_INIT( unsigned long Rcc::mPllQ );
#endif
__NO_INIT( unsigned long Rcc::mSysClk );
__NO_INIT( unsigned long Rcc::mAHBClk );
__NO_INIT( unsigned long Rcc::mAPB1Clk );
__NO_INIT( unsigned long Rcc::mAPB2Clk );

//#if !defined(STM32F37X)

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
      
#if defined(STM32F37X)
#warning RCC clock measurement not implemented!!! HSE hardcoded to 8 MHz
//            TIM_TypeDef *tim11 = TIM14;
      
      hseValue = 8000000;
      
#else
      
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
            hseValue *= 1000000;
        }        
#endif
        
#if defined(STM32F37X)
        // calc pllM, pllN, etc...
        if (hseValue > 26000000)
            hseValue = 16000000;
        mHseValue = hseValue;
        
        int mul = sysClk / mHseValue;
        if (mul * mHseValue == sysClk)
        {
            mPllM = 1; // divisor
            mPllN = mul;
        }
        else
        {
            // ???? ???? ??????????? ?????? ????????????? PLL
            // ???? ????, ?????????? ????
            // ? USB ???? ??????, ??? ?????? 1/1 (sysClk=48 ???) ??? 1/1.5 (sysClk=72 ???)
            throw Exception::badSoBad;
        }
        
        if (mPllN > 16)
            throw Exception::badSoBad;

        mSysClk = mHseValue / mPllM * mPllN;
        //end of calc
#else
        // calc pllM, pllN, etc...
        unsigned long pllvco = sysClk << 1;
        if (hseValue > 26000000)
            hseValue = 16000000;
        mHseValue = hseValue;
        mPllM = mHseValue / 1000000;
        if (mPllM > 0x3F)
            throw Exception::BadSoBad;
        mPllN = pllvco / 1000000;
        if (mPllN > 0x1FF)
            throw Exception::BadSoBad;
        mPllP = 2;
        mPllQ = pllvco / 48000000;

        mSysClk = mHseValue / mPllM * mPllN / mPllP;
        //end of calc
#endif
        
      
        /* Select regulator voltage output Scale 1 mode */
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
#if !defined(STM32F37X)        
        PWR->CR |= PWR_CR_VOS;
#endif

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

    #if defined (STM32F401xx) || defined (STM32F37X)
        /* PCLK2 = HCLK / 2*/
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
        mAPB2Clk = mAHBClk;
        
        /* PCLK1 = HCLK / 4*/
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
        mAPB1Clk = mAHBClk >> 1;
    #endif /* STM32F401xx */
       
    #if defined(STM32F37X)
        RCC->CFGR2 = mPllM - 1;
        RCC->CFGR &= ~(RCC_CFGR_PLLMULL);
        RCC->CFGR |= RCC_CFGR_PLLSRC; // 
        RCC->CFGR |= (mPllN - 2) << 18;
    #else
        /* Configure the main PLL */
        RCC->PLLCFGR = mPllM | (mPllN << 6) | (((mPllP >> 1) -1) << 16) |
                       (RCC_PLLCFGR_PLLSRC_HSE) | (mPllQ << 24);
    #endif

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
        
    #if defined (STM32F37X)
        // set SDADC clock prescaler
        unsigned long clkSDADC = mAHBClk / 6000000;
        RCC->CFGR &= ~(RCC_CFGR_SDADCPRE);
        RCC->CFGR |= (0x10 | ((clkSDADC>>1) - 1)) << 27;
        /* Enable Prefetch Buffer and set Flash Latency */
        FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY_1;
    #endif

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
        throw Exception::BadSoBad;
    }
}

//#else
//
//void Rcc::configPll(unsigned long hseValue, unsigned long sysClk)
//{
//  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
//
///******************************************************************************/
///*            PLL (clocked by HSE) used as System clock source                */
///******************************************************************************/
//
//  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------*/
//  /* Enable HSE */
//  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
// 
//  /* Wait till HSE is ready and if Time out is reached exit */
//  do
//  {
//    HSEStatus = RCC->CR & RCC_CR_HSERDY;
//    StartUpCounter++;
//  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));
//
//  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
//  {
//    HSEStatus = (uint32_t)0x01;
//  }
//  else
//  {
//    HSEStatus = (uint32_t)0x00;
//  }
//
//  if (HSEStatus == (uint32_t)0x01)
//  {
//    /* Enable Prefetch Buffer and set Flash Latency */
//    FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY_1;
// 
//     /* HCLK = SYSCLK / 1 */
//     RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
//       
//     /* PCLK2 = HCLK / 1 */
//     RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
//     
//     /* PCLK1 = HCLK / 2 */
//     RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
//
//    /* PLL configuration */
//    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
//    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL9);
//
//    /* Enable PLL */
//    RCC->CR |= RCC_CR_PLLON;
//
//    /* Wait till PLL is ready */
//    while((RCC->CR & RCC_CR_PLLRDY) == 0)
//    {
//    }
//    
//    /* Select PLL as system clock source */
//    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
//    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
//
//    /* Wait till PLL is used as system clock source */
//    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
//    {
//    }
//  }
//  else
//  { /* If HSE fails to start-up, the application will have wrong clock
//         configuration. User can add here some code to deal with this error */
//  }
//}
//
//#endif
