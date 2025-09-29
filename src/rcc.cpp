#include "rcc.h"
#include "core/core.h"

#if defined(STM32F7)
    #define DCKCFGR DCKCFGR1
#endif

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
    #if defined(STM32F4) || defined(STM32F7) // noKa 4To only for F4
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
    volatile uint32_t *CR = &RCC->CR;
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
#if !defined(STM32L4)
    case LSE:
        CR = &RCC->BDCR;
        mask = RCC_BDCR_LSEON;
        break;
    case LSI:
        CR = &RCC->CSR;
        mask = RCC_CSR_LSION;
#endif
    default: return false;
    };

    if (enabled)
    {
        *CR |= mask;
        uint32_t timeout = 50000;
        while (!isReady(src) && timeout--);
        if (!timeout)
            *CR &= ~mask;
        return timeout;
    }

    *CR &= ~mask;
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
#if !defined(STM32L4)
    case LSE: return RCC->BDCR & RCC_BDCR_LSERDY;
    case LSI: return RCC->CSR & RCC_CSR_LSIRDY;
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

#if defined(STM32F4) || defined(STM32F7)
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

    // Select regulator voltage output scale mode        
    uint32_t vos;
    bool overdrive = false;
#if defined (STM32F427_437xx) || defined (STM32F429xx)
    vos = 3;
    if (sysClk <= 120000000)
        vos = 1;
    else if (sysClk <= 144000000)
        vos = 2;
    else if (sysClk > 168000000)
        overdrive = true;
#elif defined(STM32F4)
    vos = 1;
    if (sysClk <= 144000000)
        vos = 0;
#elif defined(STM32F7)        
    vos = 3;
    if (sysClk <= 144000000)
        vos = 1;
    else if (sysClk <= 168000000)
        vos = 2;
    else if (sysClk > 180000000)
        overdrive = true;
#endif

#if defined(STM32F7)
    #define CR      CR1
    #define CSR     CSR1
#endif    
    
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    // apply voltage output scale mode, can be modified when the PLL is OFF only!
    PWR->CR = PWR->CR & ~GLUE3(PWR_,CR,_VOS) | (vos << GLUE3(PWR_,CR,_VOS_Pos));

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

    //! @todo implement uniform configuring of all PLLs and use it to configure main PLL
    
        /* Configure the main PLL */
    RCC->PLLCFGR = mPllM | (mPllN << 6) | (((mPllP >> 1) -1) << 16) | (mPllQ << 24);

    if (mHseValue) // if HSE is present make it the clock source of PLL
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

    setEnabled(PLL, true);
        
    #if defined (STM32F427_437xx) || defined (STM32F429xx) || defined(STM32F7)
    if (overdrive)
    {
        PWR->CR |= GLUE3(PWR_,CR,_ODEN);
        while (!(PWR->CSR & GLUE3(PWR_,CSR,_ODRDY)));
        PWR->CR |= GLUE3(PWR_,CR,_ODSWEN);
        while(!(PWR->CSR & GLUE3(PWR_,CSR,_ODSWRDY)));
    }
    #endif
    
#undef CR
#undef CSR
    
    // calculate latency assuming maximum voltage range (2.7 V - 3.6 V)!
    uint32_t latency = (sysClk - 1) / 30000000;
       
    // configure Flash prefetch, Instruction cache, Data cache and wait state
#if defined (STM32F401xx)
    //! @todo check the need of separate settings for STM32F401xx
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_2WS;
#elif defined(STM32F7)
    FLASH->ACR = FLASH_ACR_ARTEN | latency;
#else
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | latency;
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

void Rcc::configPll(ClockSource pll, int freqP, int freqQ, int freqR)
{
    volatile PllCfgr *cfgr = nullptr;
    switch (pll)
    {
//    case PLL: cfgr = *reinterpret_cast<PllCfgr*>(RCC->PLLCFGR); break; // not implemented here
#if defined(RCC_CR_PLLI2SON)
    case PLLI2S: cfgr = reinterpret_cast<volatile PllCfgr *>(&RCC->PLLI2SCFGR); break;
#endif
#if defined(RCC_CR_PLLSAION)
    case PLLSAI: cfgr = reinterpret_cast<volatile PllCfgr *>(&RCC->PLLSAICFGR); break;
#endif
    default: return;
    }
    
    if (isReady(pll))
        setEnabled(pll, false);        
    
    int pllin = 1000000; // PLL input frequency
    int R, Q, N = 0;
    for (Q=2; Q<16 && !N; Q++)
    {
        int Nq = Q * freqQ / pllin;
        for (R=2; R<8 && !N; R++)
        {
            int Nr = R * freqR / pllin;
            if (Nq == Nr)
                N = Nr;
        }
    }
    
    if (N)
    {
        cfgr->N = N;
        cfgr->Q = --Q;
        cfgr->R = --R;
        setEnabled(pll, true);
    }
    else
    {
        THROW(Exception::OutOfRange);
    }
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
    RCC->DCKCFGR = (RCC->DCKCFGR & ~GLUE3(RCC_,DCKCFGR,_PLLSAIDIVR_Msk)) |
                   (plldivr << GLUE3(RCC_,DCKCFGR,_PLLSAIDIVR_Pos));

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

bool Rcc::configRtc(ClockSource clock)
{   
    bool r = true;
    if (!isReady(clock))
        r &= setEnabled(clock, true);
    if (!r)
        return false;
    RCC->BDCR &= ~RCC_BDCR_RTCSEL_Msk;
    switch (clock)
    {
    case LSE: RCC->BDCR |= RCC_BDCR_RTCSEL_0; break;
    case LSI: RCC->BDCR |= RCC_BDCR_RTCSEL_1; break;
//    case HSE: RCC->BDCR |= RCC_BDCR_RTCSEL_0 | RCC_BDCR_RTCSEL_1; break;
    /// @todo config HSE prescaler to enable HSE as source clock!
    default: return false;
    }
    RCC->BDCR |= RCC_BDCR_RTCEN;
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
  /**** something old ****/
//#if defined(STM32F37X)
//        // calc pllM, pllN, etc...
//        int mul = sysClk / inputClk;
//        if (mul * inputClk == sysClk)
//        {
//            mPllM = 1; // divisor
//            mPllN = mul;
//        }
//        else
//        {
//            // влом пока продумывать расчёт коэффициентов PLL
//            // если надо, придумайте сами
//            // с USB ваще борода, там только 1/1 (sysClk=48 МГц) или 1/1.5 (sysClk=72 Мгц)
//            THROW(Exception::BadSoBad);
//        }
//
//        if (mPllN > 16)
//            THROW(Exception::BadSoBad);
//
//        mSysClk = inputClk / mPllM * mPllN;
//        //end of calc
//#endif
//    #if defined(STM32F37X)
//        RCC->CFGR2 = mPllM - 1;
//        RCC->CFGR &= ~(RCC_CFGR_PLLMULL);
//        RCC->CFGR |= RCC_CFGR_PLLSRC; //
//        RCC->CFGR |= (mPllN - 2) << 18;
//    #endif
    
//    uint32_t inputClk = mHseValue;
//    if (!inputClk)
//        inputClk = hsiValue();
//
//    if (systemClockSource() == PLL)
//    {
//        if (!setSystemClockSource(HSI))
//            return false;
//        FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk);// | FLASH_ACR_LATENCY_0WS;
//        setEnabled(PLL, false);
//    }
//
//    uint32_t pllR = 2;
//    uint32_t pllvco = sysClk * pllR;
//
//    mPllM = inputClk / 4000000;
//    if (!mPllM)
//        mPllM = 1;
//    if (mPllM > 16)
//        THROW(Exception::BadSoBad);
//    mPllN = pllvco / (inputClk / mPllM);
//    if (mPllN < 8 || mPllN > 127)
//        THROW(Exception::BadSoBad);
//    mPllP = 2;
//    mPllQ = pllvco / 48000000;
//
//    //! @todo USB clock poorly matches 48 MHz
//
//    mSysClk = inputClk;
//    mAHBClk = mSysClk;
//    mAPB1Clk = mSysClk;
//    mAPB2Clk = mSysClk;
//
//    if (sysClk >= 150000000)
//        PWR->CR5 &= ~PWR_CR5_R1MODE;
//    else
//        PWR->CR5 |= PWR_CR5_R1MODE;
//
//    RCC->PLLCFGR = ((mPllM - 1) << 4) | ((mPllN & 0x7F) << 8) /*| (((mPllQ>>1)-1) << 21) */| (((pllR>>1)-1) << 25) | (RCC_PLLCFGR_PLLREN); // | (RCC_PLLCFGR_PLLQEN);
//
//    if (mHseValue) // if HSE is present make it the clock source of PLL
//        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
//
//    if (setEnabled(PLL, true))
//    {
//        uint32_t acr = FLASH->ACR;
//        acr |= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
//        acr = (acr & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_4WS;
//        FLASH->ACR = acr;
    
//   #if defined (STM32F37X)
//        // set SDADC clock prescaler
//        unsigned long clkSDADC = mAHBClk / 6000000;
//        RCC->CFGR &= ~(RCC_CFGR_SDADCPRE);
//        RCC->CFGR |= (0x10 | ((clkSDADC>>1) - 1)) << 27;
//        /* Enable Prefetch Buffer and set Flash Latency */
//        FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY_1;
//    #endif    
    
//
//        if (setSystemClockSource(PLL))
//        {
//            mSysClk = inputClk / mPllM * mPllN / pllR;
//            mAHBClk = mSysClk;
//            mAPB1Clk = mSysClk;
//            mAPB2Clk = mSysClk;
//        }
//        else
//            THROW(Exception::BadSoBad);
//    }
////    else if (mHseValue && setEnabled(HSE, true)) {}
//    else
//        THROW(Exception::BadSoBad);
/////////*********************
  
  mSysClk  = mHseValue == 16000000? sysClk: 68812800;
  mAPB1Clk = mSysClk;         // только под кварц 14.7456
  mAPB2Clk = mSysClk;
/////////////////*************
      RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR & RCC_CR_HSERDY));

  // Настройка флэш-памяти на 2 цикла ожидания, если частота SYSCLK выше 48 МГц
  FLASH->ACR |= FLASH_ACR_LATENCY_1;

  // Настройка коэффициентов PLL
  RCC->CFGR &= ~RCC_CFGR_PLLMUL; // Сброс множителя PLL
  if(mHseValue==16000000)
    // Для HSE 14.7456 МГц, чтобы получить SYSCLK = 72 МГц, множитель PLL должен быть 14\3 (PLLMUL x 5)
     RCC->CFGR |= RCC_CFGR_PLLMUL9;
    else
  // Для HSE 14.7456 МГц, чтобы получить SYSCLK = 72 МГц, множитель PLL должен быть 14\3 (PLLMUL x 5)
  RCC->CFGR |= RCC_CFGR_PLLMUL14;

  RCC->CFGR &= ~RCC_CFGR_PLLSRC; // Выбор HSE как источника для PLL
  RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;
 
  if(mHseValue==16000000)
    RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV2;
    else
  RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV3;

  // Включение PLL и ожидание его готовности
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY));

  // Выбор PLL как источника SYSCLK и ожидание его выбора
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  // Настройка делителей AHB, APB1 и APB2
  RCC->CFGR &= ~RCC_CFGR_HPRE; // AHB prescaler = 1
  RCC->CFGR &= ~RCC_CFGR_PPRE1; // APB1 prescaler = 2 (36 МГц максимум для APB1)
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
  RCC->CFGR &= ~RCC_CFGR_PPRE2; // APB2 prescaler = 1
  
    return true;
}

bool Rcc::measureHseFreq()
{
    /// @todo implement HSE measurement on TIM16 or TIM17
    return false;
}



#elif defined(STM32F0)

bool Rcc::configPll(uint32_t sysClk)
{
  /////////*********************
  mSysClk  = 47923200;
  mAPB1Clk = 47923200;         // только под кварц 14.7456
  mAPB2Clk = 47923200;
  mAHBClk  = 47923200;
/////////////////*************
      RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR & RCC_CR_HSERDY));

  // Настройка флэш-памяти на 2 цикла ожидания, если частота SYSCLK выше 48 МГц
  FLASH->ACR |= FLASH_ACR_LATENCY;

  // Настройка коэффициентов PLL
  RCC->CFGR &= ~RCC_CFGR_PLLMUL; // Сброс множителя PLL
  // Для HSE 14.7456 МГц, чтобы получить SYSCLK = 72 МГц, множитель PLL должен быть 14\3 (PLLMUL x 5)
  RCC->CFGR |= RCC_CFGR_PLLMUL13;

  RCC->CFGR &= ~RCC_CFGR_PLLSRC; // Выбор HSE как источника для PLL
  RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;
 
  
  RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV4;

  // Включение PLL и ожидание его готовности
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY));

  // Выбор PLL как источника SYSCLK и ожидание его выбора
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  // Настройка делителей AHB, APB1 и APB2
  RCC->CFGR &= ~RCC_CFGR_HPRE; // AHB prescaler = 1
  RCC->CFGR &= ~RCC_CFGR_PPRE; // APB1 prescaler = 2 (36 МГц максимум для APB1)
 
  
    return true;
}

bool Rcc::measureHseFreq()
{
    /// @todo implement HSE measurement on TIM16 or TIM17
    return false;
}
#endif

typedef enum
{
#if defined(STM32F4) || defined(STM32F7)
    // AHB1
    RccGPIOA    = 1 << 0,   
    RccGPIOB    = 1 << 1,
    RccGPIOC    = 1 << 2,
    RccGPIOD    = 1 << 3,
    RccGPIOE    = 1 << 4,
    RccGPIOF    = 1 << 5,
    RccGPIOG    = 1 << 6,
    RccGPIOH    = 1 << 7,
    RccGPIOI    = 1 << 8,
    RccGPIOJ    = 1 << 9,
    RccGPIOK    = 1 << 10,
    RccCRC      = 1 << 12,
    RccBKPSRAM  = 1 << 18,
    RccCCM      = 1 << 20, // CCM data RAM
    RccDMA1     = 1 << 21,
    RccDMA2     = 1 << 22,
    RccDMA2D    = 1 << 23,
    RccETH      = (1<<25) | (1<<26) | (1<<27) | (1<<28),
    RccOTGHS    = 1 << 29,
    RccOTGHSULPI = 1 << 30,
    // AHB2
    RccDCMI     = 1 << 0,
    RccCRYP     = 1 << 4,
    RccHASH     = 1 << 5,
    RccRNG      = 1 << 6,
    RccOTGFS    = 1 << 7,
    // AHB3
    RccFMC      = 1 << 0,
    RccADC1     = 0,
    RccADC2     = 0,
    RccADC3     = 0,
    RccADC4     = 0,
    RccDAC1     = 0,
    RccDAC2     = 0,
    RccDAC3     = 0,
    RccDAC4     = 0,
    
#elif defined(STM32G4)
    // AHB1
    RccDMA1     = 1 << 0,
    RccDMA2     = 1 << 1,
    RccDMAMUX1  = 1 << 2,
    RccCORDIC   = 1 << 3,
    RccFMAC     = 1 << 4,
    RccFLASH    = 1 << 8,
    RccCRC      = 1 << 12,
    // AHB2
    RccGPIOA    = 1 << 0,   
    RccGPIOB    = 1 << 1,
    RccGPIOC    = 1 << 2,
    RccGPIOD    = 1 << 3,
    RccGPIOE    = 1 << 4,
    RccGPIOF    = 1 << 5,
    RccGPIOG    = 1 << 6,
    RccGPIOH    = 1 << 7,
    RccGPIOI    = 1 << 8,
    RccGPIOJ    = 1 << 9,
    RccGPIOK    = 1 << 10,
    RccADC1     = 1 << 13,
    RccADC2     = 1 << 13,
    RccADC3     = 1 << 14,
    RccADC4     = 1 << 14,
    RccADC5     = 1 << 14,
    RccDAC1     = 1 << 16,
    RccDAC2     = 1 << 17,
    RccDAC3     = 1 << 18,
    RccDAC4     = 1 << 19,
    RccAES      = 1 << 24,
    RccRNG      = 1 << 26,
    // AHB3
    RccFMC      = 1 << 0,
    RccQSPI     = 1 << 8,
    
#elif defined(STM32L4)
    // AHB1
    RccDMA1     = 1 << 0,
    RccDMA2     = 1 << 1,
    RccFLASH    = 1 << 8,
    RccCRC      = 1 << 12,
    RccTSC      = 1 << 16,
    RccDMA2D    = 1 << 17,
    // AHB2
    RccGPIOA    = 1 << 0,   
    RccGPIOB    = 1 << 1,
    RccGPIOC    = 1 << 2,
    RccGPIOD    = 1 << 3,
    RccGPIOE    = 1 << 4,
    RccGPIOF    = 1 << 5,
    RccGPIOG    = 1 << 6,
    RccGPIOH    = 1 << 7,
    RccGPIOI    = 1 << 8,
    RccGPIOJ    = 1 << 9,
    RccGPIOK    = 1 << 10,
    RccOTGFS    = 1 << 12,
    RccADC1     = 1 << 13,
    RccADC2     = 1 << 13,
    RccADC3     = 1 << 13,
    RccDCMI     = 1 << 14,
    RccAES      = 1 << 16,
    RccHASH     = 1 << 17,
    RccRNG      = 1 << 18,
    // AHB3
    RccFMC      = 1 << 0,
    RccQSPI     = 1 << 8,
    RccDAC1     = 0,
    RccDAC2     = 0,
    RccDAC3     = 0,
    RccDAC4     = 0,
    
#elif defined(STM32F3) || defined(STM32F0)
    RccGPIOA    = 1 << 17,   
    RccGPIOB    = 1 << 18,
    RccGPIOC    = 1 << 19,
    RccGPIOD    = 1 << 20,
    RccGPIOE    = 1 << 21,
    RccGPIOF    = 1 << 22,
    RccGPIOG    = 1 << 23,
    RccGPIOH    = 1 << 16,
    RccGPIOI    = 0,
    RccGPIOJ    = 0,
    RccGPIOK    = 0,
    RccDMA1     = 1 << 0,
    RccDMA2     = 1 << 1,
    RccFLITF    = 1 << 4,
    RccFMC      = 1 << 5,
    RccCRC      = 1 << 6,
    RccTSC      = 1 << 24,
    RccADC1     = 1 << 28,
    RccADC2     = 1 << 28,
    RccADC3     = 1 << 29,
    RccADC4     = 1 << 29,
    RccDAC1     = 0,
    RccDAC2     = 0,
    RccDAC3     = 0,
    RccDAC4     = 0,
    
#endif
} RccEnableBit;

#if defined(STM32F4) || defined(STM32G4) || defined(STM32L4) || defined(STM32F7)
#define AHB3PERIPH_BASE 0x60000000
#endif

void Rcc::setPeriphEnabled(void *periphBase, bool enabled)
{
    uint32_t base = reinterpret_cast<uint32_t>(periphBase);
    uint32_t bus = periphBusBase(periphBase);
    uint32_t offset = periphBusOffset(periphBase);
    uint32_t mask1 = 1 << (offset >> 10);
#if defined(STM32G4) || defined(STM32L4)
    uint32_t mask2 = 1 << ((offset >> 10) - 32);
#endif
    
#if defined(STM32F3) 
    #define AHB1ENR     AHBENR
    #define AHB2ENR     AHBENR
    #define AHB3ENR     AHBENR
#elif defined(STM32G4) || defined(STM32L4)
    #define APB1ENR     APB1ENR1
#elif defined(STM32F0)
    #define AHB1ENR     AHBENR
    #define AHB2ENR     AHBENR
    #define AHB3ENR     AHBENR
    
    #define AHB1PERIPH_BASE  AHBPERIPH_BASE
    #define AHB3PERIPH_BASE  23
    #define APB1PERIPH_BASE  APBPERIPH_BASE
    #define APB2PERIPH_BASE  42
#endif
    
    volatile uint32_t *ENR = nullptr;
    switch (bus)
    {
        case AHB1PERIPH_BASE: ENR = &RCC->AHB1ENR; break;
        case AHB2PERIPH_BASE: ENR = &RCC->AHB2ENR; break;
        case AHB3PERIPH_BASE: ENR = &RCC->AHB3ENR; break;
        case APB1PERIPH_BASE: ENR = &RCC->APB1ENR; break;
        case APB2PERIPH_BASE: ENR = &RCC->APB2ENR; break;
        //! @todo deal with APB1ENR2 register in some MCUs
    }
    
    //! @todo check for other STM families!

    if (base >= AHB1PERIPH_BASE)
    {
        switch (base)
        {
    #if defined(GPIOA)
        case GPIOA_BASE:    *ENR |= RccGPIOA; break;
    #endif
    #if defined(GPIOB) 
        case GPIOB_BASE:    *ENR |= RccGPIOB; break;
    #endif
    #if defined(GPIOC) 
        case GPIOC_BASE:    *ENR |= RccGPIOC; break;
    #endif
    #if defined(GPIOD) 
        case GPIOD_BASE:    *ENR |= RccGPIOD; break;
    #endif
    #if defined(GPIOE) 
        case GPIOE_BASE:    *ENR |= RccGPIOE; break;
    #endif
    #if defined(GPIOF) 
        case GPIOF_BASE:    *ENR |= RccGPIOF; break;
    #endif
    #if defined(GPIOG) 
        case GPIOG_BASE:    *ENR |= RccGPIOG; break;
    #endif
    #if defined(GPIOH) 
        case GPIOH_BASE:    *ENR |= RccGPIOH; break;
    #endif
    #if defined(GPIOI) 
        case GPIOI_BASE:    *ENR |= RccGPIOI; break;
    #endif
    #if defined(GPIOJ) 
        case GPIOJ_BASE:    *ENR |= RccGPIOJ; break;
    #endif
    #if defined(GPIOK) 
        case GPIOK_BASE:    *ENR |= RccGPIOK; break;
    #endif 
    #if defined(DMA1)
        case DMA1_BASE:     *ENR |= RccDMA1; break;
    #endif
    #if defined(DMA2)
        case DMA2_BASE:     *ENR |= RccDMA2; break;
    #endif
    #if defined(ADC1)
        case ADC1_BASE:     *ENR |= RccADC1; break;
    #endif
    #if defined(ADC2)    
        case ADC2_BASE:     *ENR |= RccADC2; break;
    #endif
    #if defined(ADC3)
        case ADC3_BASE:     *ENR |= RccADC3; break;
    #endif
    #if defined(ADC4)
        case ADC4_BASE:     *ENR |= RccADC4; break;
    #endif
    #if defined(ADC5)
        case ADC5_BASE:     *ENR |= RccADC5; break;
    #endif
    #if defined(DAC1_BASE)
        case DAC1_BASE:     *ENR |= RccDAC1; break;
    #endif
    #if defined(DAC2)    
        case DAC2_BASE:     *ENR |= RccDAC2; break;
    #endif
    #if defined(DAC3)
        case DAC3_BASE:     *ENR |= RccDAC3; break;
    #endif
    #if defined(DAC4)
        case DAC4_BASE:     *ENR |= RccDAC4; break;
    #endif        
        
    #ifdef DMA2D
        case DMA2D_BASE:    *ENR |= RccDMA2D; break;
    #endif
    #ifdef ETH
        case ETH_BASE:      *ENR |= RccETH; break;
    #endif
        
    #if defined(FMC_BASE)
        case FMC_BASE:      *ENR |= RccFMC; break;
    #endif
        }
    }
    else switch (base)
    {
#if defined(STM32G4)
    case FDCAN2_BASE:
    case FDCAN3_BASE:
        RCC->APB1ENR1 |= RCC_APB1ENR1_FDCANEN;
        break;
#elif defined(STM32F3)
    #if defined(DAC2)
    case DAC2_BASE:
        RCC->APB1ENR |= RCC_APB1ENR_DAC2EN;
    #endif
#endif
        
    //! @todo Fill other cases

    default:
        if (bus == APB1PERIPH_BASE)
        {
#if defined(STM32F4) || defined(STM32F7)  || defined(STM32F3)
            RCC->APB1ENR |= mask1;
#elif defined(STM32G4) || defined(STM32L4)
            RCC->APB1ENR1 |= mask1;
            RCC->APB1ENR2 |= mask2;
#endif            
        }
        else if (bus == APB2PERIPH_BASE)
        {
            RCC->APB2ENR |= mask1;
        }
    }
}

void Rcc::resetPeriph(void *periphBase)
{
    //! @todo Make the same as enablePeriph
    
    uint32_t base = reinterpret_cast<uint32_t>(periphBase);
    uint32_t bus = periphBusBase(periphBase);
    uint32_t offset = periphBusOffset(periphBase);
    uint32_t mask1 = 1 << (offset >> 10);
#if defined(STM32G4) || defined(STM32L4)
    uint32_t mask2 = 1 << ((offset >> 10) - 32);
#endif
    
    switch (base)
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
    default:
        if (bus == APB1PERIPH_BASE)
        {
#if defined(STM32F4) || defined(STM32F7)
            RCC->APB1RSTR |= mask1;
            RCC->APB1RSTR &= ~mask1;
#elif defined(STM32G4) || defined(STM32L4)
            RCC->APB1RSTR1 |= mask1;
            RCC->APB1RSTR2 |= mask2;
            RCC->APB1RSTR1 &= ~mask1;
            RCC->APB1RSTR2 &= ~mask2;
#endif                        
        }
        else if (bus == APB2PERIPH_BASE)
        {
            RCC->APB2RSTR |= 1 << (offset >> 10);
        }
    }
}

int Rcc::getPeriphClk(void *periphBase)
{
    uint32_t bus = periphBusBase(periphBase);
    switch (bus)
    {
    case APB1PERIPH_BASE: return pClk1();
    case APB2PERIPH_BASE: return pClk2();
    default: return hClk();
    }
}
                         
uint32_t Rcc::periphBusBase(void *periph)
{
    uint32_t periphBase = reinterpret_cast<uint32_t>(periph);
    if (periphBase >= 0xE0000000)
        return 0;
    if (periphBase >= AHB3PERIPH_BASE)
        return AHB3PERIPH_BASE;
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

