#include "rcc.h"
#include "core/core.h"

Rcc *Rcc::m_self = nullptr;

Rcc *Rcc::instance()
{
    if (!m_self)
        m_self = new Rcc();
    return m_self;
}

Rcc &rcc()
{
    return *Rcc::instance();
}

Rcc::Rcc() :
    mHseValue(0),
    mPllM(0),
    mPllN(0),
#if !defined(STM32F37X)
    mPllP(0),
    mPllQ(0),
#endif
    mSysClk(0),
    mAHBClk(0),
    mAPB1Clk(0),
    mAPB2Clk(0)
{
    #if defined(STM32F4) // noKa 4To only for F4
    measureHseFreq();
    #endif
}

bool Rcc::setSystemClockSource(ClockSource src)
{
    if (src > PLL)
        return false;

    uint32_t sws = src << RCC_CFGR_SWS_Pos;

    // if the clock is already in use
    if ((RCC->CFGR & RCC_CFGR_SWS) == sws)
        return true; // do nothing

    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |= src;

    uint32_t timeout = 50000;
    while (((RCC->CFGR & RCC_CFGR_SWS) != sws) && timeout--);

    return timeout;
}

Rcc::ClockSource Rcc::systemClockSource() const
{
    return static_cast<ClockSource>((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos);
}

bool Rcc::setEnabled(ClockSource src, bool enabled)
{
    uint32_t mask = 0;
    switch (src)
    {
#if defined(STM32L4)
    case MSI: mask = RCC_CR_MSION; break;
#endif
    case HSI: mask = RCC_CR_HSION; break;
    case HSE: mask = RCC_CR_HSEON; break;
    case PLL: mask = RCC_CR_PLLON; break;
#if defined(RCC_CR_PLLI2SON)
    case PLLI2S: mask = RCC_CR_PLLI2SON; break;
#endif
#if defined(RCC_CR_PLLSAION)
    case PLLSAI: mask = RCC_CR_PLLSAION; break;
#endif
    default: return false;
    };

    if (enabled)
    {
        RCC->CR |= mask;
        uint32_t timeout = 50000;
        while (!isReady(src) && timeout--);
        if (!timeout)
            RCC->CR &= ~mask;
        return timeout;
    }

    RCC->CR &= ~mask;
    return true;
}

bool Rcc::isReady(ClockSource src)
{
    switch (src)
    {
#if defined(STM32L4)
    case MSI: return RCC->CR & RCC_CR_MSIRDY;
#endif
    case HSI: return RCC->CR & RCC_CR_HSIRDY;
    case HSE: return RCC->CR & RCC_CR_HSERDY;
    case PLL: return RCC->CR & RCC_CR_PLLRDY;
#if defined(RCC_CR_PLLI2SON)
    case PLLI2S: return RCC->CR & RCC_CR_PLLI2SRDY;
#endif
#if defined(RCC_CR_PLLSAION)
    case PLLSAI: return RCC->CR & RCC_CR_PLLSAIRDY;
#endif
    default: return false;
    };
}

bool Rcc::configPll(uint32_t hseValue, uint32_t sysClk)
{
    mHseValue = hseValue;

    if (!isReady(HSE))
    {
        bool hseEnabled = setEnabled(HSE, true);
        if (!hseEnabled)
            return false;
    }

    return configPll(sysClk);
}

#if defined(STM32F4)
bool Rcc::configPll(uint32_t sysClk)
{
    uint32_t inputClk = mHseValue;
    if (!inputClk)
        inputClk = hsiValue();

    if (systemClockSource() == PLL)
    {
        if (!setSystemClockSource(HSI))
            return false;
        setEnabled(PLL, false);
    }

#if defined(STM32F37X)
        // calc pllM, pllN, etc...
        int mul = sysClk / inputClk;
        if (mul * inputClk == sysClk)
        {
            mPllM = 1; // divisor
            mPllN = mul;
        }
        else
        {
            // влом пока продумывать расчЄт коэффициентов PLL
            // если надо, придумайте сами
            // с USB ваще борода, там только 1/1 (sysClk=48 ћ√ц) или 1/1.5 (sysClk=72 ћгц)
            THROW(Exception::BadSoBad);
        }

        if (mPllN > 16)
            THROW(Exception::BadSoBad);

        mSysClk = inputClk / mPllM * mPllN;
        //end of calc
#else
        // calc pllM, pllN, etc...
        unsigned long pllvco = sysClk << 1;
        mPllM = inputClk / 1000000;
        if (mPllM > 0x3F)
            THROW(Exception::BadSoBad);
        mPllN = pllvco / 1000000;
        if (mPllN > 0x1FF)
            THROW(Exception::BadSoBad);
        mPllP = 2;
        mPllQ = pllvco / 48000000;

        mSysClk = inputClk / mPllM * mPllN / mPllP;
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

    #if defined (STM32F401xx) || defined (STM32F37X)
        /* PCLK2 = HCLK / 2*/
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
        mAPB2Clk = mAHBClk;

        /* PCLK1 = HCLK / 4*/
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
        mAPB1Clk = mAHBClk >> 1;
    #else
        /* PCLK2 = HCLK / 2*/
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
        mAPB2Clk = mAHBClk >> 1;

        /* PCLK1 = HCLK / 4*/
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
        mAPB1Clk = mAHBClk >> 2;
    #endif /* STM32F401xx */

    #if defined(STM32F37X)
        RCC->CFGR2 = mPllM - 1;
        RCC->CFGR &= ~(RCC_CFGR_PLLMULL);
        RCC->CFGR |= RCC_CFGR_PLLSRC; //
        RCC->CFGR |= (mPllN - 2) << 18;
    #else
        /* Configure the main PLL */
        RCC->PLLCFGR = mPllM | (mPllN << 6) | (((mPllP >> 1) -1) << 16) | (mPllQ << 24);

        if (mHseValue) // if HSE is present make it the clock source of PLL
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
    #endif


        setEnabled(PLL, true);

    #if defined (STM32F427_437xx) || defined (STM32F429xx)
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
    #elif defined (STM32F401xx)
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
    #else
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
    #endif /* STM32F401xx */

    #if defined (STM32F37X)
        // set SDADC clock prescaler
        unsigned long clkSDADC = mAHBClk / 6000000;
        RCC->CFGR &= ~(RCC_CFGR_SDADCPRE);
        RCC->CFGR |= (0x10 | ((clkSDADC>>1) - 1)) << 27;
        /* Enable Prefetch Buffer and set Flash Latency */
        FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY_1;
    #endif

        return setSystemClockSource(PLL);

        /*     The original configuration procedure     *\
        |  Use it for reference if something goes wrong  |
        \*                                              */

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

bool Rcc::measureHseFreq()
{
    mHseValue = 0;

    // Enable HSE
    RCC->CR |= RCC_CR_HSEON;

    // Wait till HSE is ready and if Time out is reached exit;
    uint32_t timeout = 20000;
    while (!(RCC->CR & RCC_CR_HSERDY) && timeout--);

    if (!timeout) // HSE doesn't work :c
    {
        // Disable HSE
        RCC->CR &= ~RCC_CR_HSEON;
        return false;
    }

    // measure HSE frequency
    RCC->APB2ENR |= RCC_APB2ENR_TIM11EN; // enable peripheral clock for TIM11
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
    RCC->APB2ENR &= ~RCC_APB2ENR_TIM11EN; // disable peripheral clock to TIM11
    // end of measure

    // calculate hseValue
    int hseValue;
    fr1 -= fr0;
    hseValue = ((16 * 30) * loopCount + fr1 / 2) / fr1;
    hseValue *= 1000000;

    if (hseValue > 26000000)
        hseValue = 16000000;
    mHseValue = hseValue;
    return true;
}

#if defined(LTDC)
int Rcc::configLtdcClock(int frequency)
{
    if (isReady(PLLSAI))
        setEnabled(PLLSAI, false);

    //! @todo test these calculations, it's sometimes wrong :c

    int pllN = 192; // magic number by default
    int div = pllN * 1000000 / frequency;
    div = (div + 1) & ~1; // make even
    pllN = div * frequency / 1000000;
    int plldiv = upper_power_of_two((div + 6) / 7);
    if (plldiv < 2)
        plldiv = 2;
    while (plldiv > 16)
    {
        plldiv >>= 1;
        pllN >>= 1;
    }
    int pllR = div / plldiv;
    uint32_t pllsai = RCC->PLLSAICFGR & (RCC_PLLSAICFGR_PLLSAIQ_Msk);
    RCC->PLLSAICFGR = pllsai | (pllN << RCC_PLLSAICFGR_PLLSAIN_Pos)
                             | (pllR << RCC_PLLSAICFGR_PLLSAIR_Pos);
    int plldivr = 0;
    switch (plldiv)
    {
    case 2:  plldivr = 0; break;
    case 4:  plldivr = 1; break;
    case 8:  plldivr = 2; break;
    case 16: plldivr = 3; break;
    default: THROW(Exception::OutOfRange);
    }
    RCC->DCKCFGR = (RCC->DCKCFGR & ~RCC_DCKCFGR_PLLSAIDIVR_Msk) |
                   (plldivr << RCC_DCKCFGR_PLLSAIDIVR_Pos);

    setEnabled(PLLSAI, true);

    return pllN * 1000000 / (pllR * plldiv);
}
#endif

void Rcc::configClockOutput(Gpio::Config mco, ClockSource clk, int prescaler)
{
    if (prescaler <= 1)
        prescaler = 0;
    else if (prescaler > 1 && prescaler <= 5)
        prescaler += 2;
    else
        return; // wrong configuration

    uint32_t cfgr = RCC->CFGR;

    if (mco == Gpio::MCO1_PA8)
    {
        cfgr &= ~(RCC_CFGR_MCO1_Msk | RCC_CFGR_MCO1PRE_Msk);
        cfgr |= prescaler << RCC_CFGR_MCO1PRE_Pos;
        switch (clk)
        {
        case HSI: cfgr |= 0 << RCC_CFGR_MCO1_Pos; break;
        case LSE: cfgr |= 1UL << RCC_CFGR_MCO1_Pos; break;
        case HSE: cfgr |= 2UL << RCC_CFGR_MCO1_Pos; break;
        case PLL: cfgr |= 3UL << RCC_CFGR_MCO1_Pos; break;
        default: return; // wrong configuration
        }
    }
    else if (mco == Gpio::MCO2_PC9)
    {
        cfgr &= ~(RCC_CFGR_MCO2_Msk | RCC_CFGR_MCO2PRE_Msk);
        cfgr |= prescaler << RCC_CFGR_MCO2PRE_Pos;
        switch (clk)
        {
        case SYSCLK: cfgr |= 0 << RCC_CFGR_MCO2_Pos; break;
        case PLLI2S: cfgr |= 1UL << RCC_CFGR_MCO2_Pos; break;
        case HSE:    cfgr |= 2UL << RCC_CFGR_MCO2_Pos; break;
        case PLL:    cfgr |= 3UL << RCC_CFGR_MCO2_Pos; break;
        default: return; // wrong configuration
        }
    }

    RCC->CFGR = cfgr;
    Gpio::config(mco);
}

#elif defined(STM32L4)

bool Rcc::configPll(uint32_t sysClk)
{
    //! @todo Dopilit RCC config for STM32L4

    uint32_t pllR = 2;
    uint32_t pllvco = sysClk * pllR;
    uint32_t clkin = 4000000; // MSI clock
//    mHseValue = clkin;
    mPllM = 1;
    mPllN = pllvco / (clkin / mPllM);
    if (mPllN > 0x7F)
        THROW(Exception::BadSoBad);
    mPllQ = pllvco / 48000000;
//    mPllP = 7;


    mSysClk = clkin / mPllM * mPllN / pllR;
    mAHBClk = mSysClk;
    mAPB1Clk = mSysClk;
    mAPB2Clk = mSysClk;

    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;

    RCC->PLLCFGR = ((mPllM - 1) << 4) | ((mPllN & 0x7F) << 8) | (((mPllQ>>1)-1) << 21) | (((pllR>>1)-1) << 25) | (RCC_PLLCFGR_PLLSRC_MSI) | (RCC_PLLCFGR_PLLREN);
    RCC->CR |= RCC_CR_PLLON;

    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR = RCC->CFGR & ~RCC_CFGR_SW | RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    return true;
}

bool Rcc::measureHseFreq()
{
    return false;
}

#elif defined(STM32G4)

bool Rcc::configPll(uint32_t sysClk)
{
    //! @todo Dopilit RCC config for STM32G4

    uint32_t inputClk = mHseValue;
    if (!inputClk)
        inputClk = hsiValue();

    if (systemClockSource() == PLL)
    {
        if (!setSystemClockSource(HSI))
            return false;
        FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_0WS;
        setEnabled(PLL, false);
    }

    uint32_t pllR = 2;
    uint32_t pllvco = sysClk * pllR;

    mPllM = inputClk / 4000000;
    if (!mPllM)
        mPllM = 1;
    if (mPllM > 16)
        THROW(Exception::BadSoBad);
    mPllN = pllvco / (inputClk / mPllM);
    if (mPllN < 8 || mPllN > 127)
        THROW(Exception::BadSoBad);
    mPllP = 2;
    mPllQ = pllvco / 48000000;

    //! @todo USB clock poorly matches 48 MHz

    mSysClk = inputClk;
    mAHBClk = mSysClk;
    mAPB1Clk = mSysClk;
    mAPB2Clk = mSysClk;

    if (sysClk >= 150000000)
        PWR->CR5 &= ~PWR_CR5_R1MODE;
    else
        PWR->CR5 |= PWR_CR5_R1MODE;

    RCC->PLLCFGR = ((mPllM - 1) << 4) | ((mPllN & 0x7F) << 8) /*| (((mPllQ>>1)-1) << 21) */| (((pllR>>1)-1) << 25) | (RCC_PLLCFGR_PLLREN); // | (RCC_PLLCFGR_PLLQEN);

    if (mHseValue) // if HSE is present make it the clock source of PLL
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

    if (setEnabled(PLL, true))
    {
        uint32_t acr = FLASH->ACR;
        acr |= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
        acr = (acr & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_4WS;
        FLASH->ACR = acr;

        if (setSystemClockSource(PLL))
        {
            mSysClk = inputClk / mPllM * mPllN / pllR;
            mAHBClk = mSysClk;
            mAPB1Clk = mSysClk;
            mAPB2Clk = mSysClk;
        }
        else
            THROW(Exception::BadSoBad);
    }
//    else if (mHseValue && setEnabled(HSE, true)) {}
    else
        THROW(Exception::BadSoBad);

    return true;
}

bool Rcc::measureHseFreq()
{
    /// @todo implement HSE measurement on TIM16 or TIM17
    return false;
}

#elif defined(STM32F3)

bool Rcc::configPll(uint32_t sysClk)
{
    uint32_t inputClk = mHseValue;
    if (!inputClk)
        inputClk = hsiValue();

    if (systemClockSource() == PLL)
    {
        if (!setSystemClockSource(HSI))
            return false;
        FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk);// | FLASH_ACR_LATENCY_0WS;
        setEnabled(PLL, false);
    }

    uint32_t pllR = 2;
    uint32_t pllvco = sysClk * pllR;

    mPllM = inputClk / 4000000;
    if (!mPllM)
        mPllM = 1;
    if (mPllM > 16)
        THROW(Exception::BadSoBad);
    mPllN = pllvco / (inputClk / mPllM);
    if (mPllN < 8 || mPllN > 127)
        THROW(Exception::BadSoBad);
    mPllP = 2;
    mPllQ = pllvco / 48000000;

    //! @todo USB clock poorly matches 48 MHz

    mSysClk = inputClk;
    mAHBClk = mSysClk;
    mAPB1Clk = mSysClk;
    mAPB2Clk = mSysClk;

    if (sysClk >= 150000000)
        PWR->CR5 &= ~PWR_CR5_R1MODE;
    else
        PWR->CR5 |= PWR_CR5_R1MODE;

    RCC->PLLCFGR = ((mPllM - 1) << 4) | ((mPllN & 0x7F) << 8) /*| (((mPllQ>>1)-1) << 21) */| (((pllR>>1)-1) << 25) | (RCC_PLLCFGR_PLLREN); // | (RCC_PLLCFGR_PLLQEN);

    if (mHseValue) // if HSE is present make it the clock source of PLL
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

    if (setEnabled(PLL, true))
    {
        uint32_t acr = FLASH->ACR;
        acr |= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
        acr = (acr & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_4WS;
        FLASH->ACR = acr;

        if (setSystemClockSource(PLL))
        {
            mSysClk = inputClk / mPllM * mPllN / pllR;
            mAHBClk = mSysClk;
            mAPB1Clk = mSysClk;
            mAPB2Clk = mSysClk;
        }
        else
            THROW(Exception::BadSoBad);
    }
//    else if (mHseValue && setEnabled(HSE, true)) {}
    else
        THROW(Exception::BadSoBad);

    return true;
}

bool Rcc::measureHseFreq()
{
    /// @todo implement HSE measurement on TIM16 or TIM17
    return false;
}


#endif

void Rcc::setPeriphEnabled(void *periphBase, bool enabled)
{
    uint32_t base = reinterpret_cast<uint32_t>(periphBase);
    uint32_t bus = periphBusBase(periphBase);
    uint32_t offset = periphBusOffset(periphBase);
    
    //! @todo check for other STM families!

    if (base == ADC2_BASE)
        RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
    else if (base == ADC3_BASE)
        RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    else if (bus == APB1PERIPH_BASE)
        RCC->APB1ENR |= 1 << (offset >> 10);
    else if (bus == APB2PERIPH_BASE)
        RCC->APB2ENR |= 1 << (offset >> 10);
    else switch (base)
    {
#ifdef DMA2D
    case DMA2D_BASE:    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN; break;
#endif
#ifdef ETH
    case ETH_BASE:      RCC->AHB1ENR |=
                        RCC_AHB1ENR_ETHMACEN | /*RCC_AHB1ENR_ETHMACPTPEN | */
                        RCC_AHB1ENR_ETHMACRXEN | RCC_AHB1ENR_ETHMACTXEN;
                        break;
#endif
    //! @todo Fill other cases
    }
}

void Rcc::resetPeriph(void *periphBase)
{
    uint32_t base = reinterpret_cast<uint32_t>(periphBase);
    uint32_t bus = periphBusBase(periphBase);
    uint32_t offset = periphBusOffset(periphBase);
    
    if (bus == APB1PERIPH_BASE)
        RCC->APB1RSTR |= 1 << (offset >> 10);
    else if (bus == APB2PERIPH_BASE)
        RCC->APB2RSTR |= 1 << (offset >> 10);
    else switch (base)
    {
#ifdef DMA2D
    case DMA2D_BASE:    RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2DRST;
                        RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2DRST;
                        break;
#endif
#ifdef ETH
    case ETH_BASE:      RCC->AHB1RSTR |= RCC_AHB1RSTR_ETHMACRST;
                        RCC->AHB1RSTR &= ~RCC_AHB1RSTR_ETHMACRST;
                        break;
#endif
    //! @todo Fill other cases
    }
}
                         
uint32_t Rcc::periphBusBase(void *periph)
{
    uint32_t periphBase = reinterpret_cast<uint32_t>(periph);
    if (periphBase >= 0xE0000000)
        return 0;
    if (periphBase >= 0x60000000)
        return 0x60000000;
    if (periphBase >= AHB2PERIPH_BASE)
        return AHB2PERIPH_BASE;
    if (periphBase >= AHB1PERIPH_BASE)
        return AHB1PERIPH_BASE;
    if (periphBase >= APB2PERIPH_BASE)
        return APB2PERIPH_BASE;
    if (periphBase >= APB1PERIPH_BASE)
        return APB1PERIPH_BASE;
    return 0;
}

uint32_t Rcc::periphBusOffset(void *periph)
{
    return reinterpret_cast<uint32_t>(periph) - periphBusBase(periph);
}

int Rcc::getPeriphClk(void *periphBase)
{
    if ((uint32_t)periphBase < 0x40010000)
        return pClk1();
    else if ((uint32_t)periphBase < 0x40020000)
        return pClk2();
    else if ((uint32_t)periphBase < 0x50000000)
        return hClk();
    return 0;
}
