#include "hardwaretimer.h"

HardwareTimer* HardwareTimer::mTimers[20] {0};

HardwareTimer::HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz)
{
    unsigned int hclk = rcc().hClk();
    unsigned int pclk1 = rcc().pClk1();
    unsigned int pclk2 = rcc().pClk2();
#if defined(STM32F446xx)
    if (RCC->DCKCFGR & RCC_DCKCFGR_TIMPRE)
    {
//        if (RCC->CFGR & RCC_CFGR_PPRE1_Msk)
        pclk1 = pclk2 = hclk;
    }
    else
#endif
    {
        if (RCC->CFGR & RCC_CFGR_PPRE1_Msk)
            pclk1 *= 2;
        if (RCC->CFGR & RCC_CFGR_PPRE2_Msk)
            pclk2 *= 2;
    }

    mInputClk = pclk2;
    switch (timerNumber)
    {
#if defined(TIM1)
      case 1:
        mTim = TIM1;
        m_caps = AdvancedControl;
        mIrq = TIM_IRQn(1_UP); // у этого таймера 4 прерывания, задаётся позже, когда нужно
        break;
#endif
#if defined(TIM2)
      case 2:
        mTim = TIM2;
        m_caps = static_cast<Capability>(GeneralPurpose1 | Res32bit);
        mIrq = TIM_IRQn(2);
        mInputClk = pclk1;
        break;
#endif
#if defined(TIM3)
      case 3:
        mTim = TIM3;
        m_caps = GeneralPurpose1;
        mIrq = TIM_IRQn(3);
        mInputClk = pclk1;
        break;
#endif
#if defined(TIM4)        
      case 4:
        mTim = TIM4;
        m_caps = GeneralPurpose1;
        mIrq = TIM_IRQn(4);
        mInputClk = pclk1;
        break;
#endif
#if defined(TIM5)
      case 5:
        mTim = TIM5;
        m_caps = static_cast<Capability>(GeneralPurpose1 | Res32bit);
        mIrq = TIM_IRQn(5);
        mInputClk = pclk1;
        break;
#endif
#if defined(TIM6)
      case 6:
        mTim = TIM6;
        m_caps = Basic;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(6);
        break;
#endif
#if defined(TIM7)
      case 7:
        mTim = TIM7;
        m_caps = Basic;
        mIrq = TIM_IRQn(7);
        mInputClk = pclk1;
        break;
#endif		
#if defined(TIM8)     
      case 8:
        mTim = TIM8;
        m_caps = AdvancedControl;
        mIrq = TIM_IRQn(8_UP); // у этого таймера 4 прерывания, задаётся позже, когда нужно
        break;
#endif
#if defined(TIM9)
      case 9:
        mTim = TIM9;
        m_caps = GeneralPurpose2;
        mIrq = TIM_IRQn(9);
        break;
#endif
#if defined(TIM10)      
      case 10:
        mTim = TIM10;
        m_caps = GeneralPurpose2;
        mIrq = TIM_IRQn(10);
        mInputClk = pclk1; // or pclk2 ???
        break;
#endif
#if defined(TIM11)
      case 11:
        mTim = TIM11;
        m_caps = GeneralPurpose2;
        mIrq = TIM_IRQn(11);
        break;
#endif
#if defined(TIM12)
      case 12:
        mTim = TIM12;
        m_caps = GeneralPurpose2;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(12);
        break;
#endif
#if defined(TIM13)
      case 13:
        mTim = TIM13;
        m_caps = GeneralPurpose2;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(13);
        break;
#endif
#if defined(TIM14)         
      case 14:
        mTim = TIM14;
        m_caps = GeneralPurpose2;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(14);
        break;
#endif
#if defined(TIM15)
      case 15:
        mTim = TIM15;
        m_caps = GeneralPurpose3;
        mIrq = TIM_IRQn(15);
        break;
#endif
#if defined(TIM16)         
      case 16:
        mTim = TIM16;
        m_caps = GeneralPurpose3;
        mIrq = TIM_IRQn(16);
        break;
#endif
#if defined(TIM17)
      case 17:
        mTim = TIM17;
        m_caps = GeneralPurpose3;
        mIrq = TIM_IRQn(17);
        break;
#endif
#if defined(TIM18)
      case 18:
        mTim = TIM18;
        m_caps = GeneralPurpose3;
        mIrq = TIM_IRQn(18);
        mInputClk = pclk1;
        break;
#endif
#if defined(TIM19)
      case 19:
        mTim = TIM19;
        m_caps = GeneralPurpose3;
        mIrq = TIM_IRQn(19);
        break;
#endif
#if defined(TIM20)         
      case 20:
        mTim = TIM20;
        m_caps = AdvancedControl;
        mIrq = TIM_IRQn(20_UP);
        break;
#endif
        
      default:
        return;
    }

    rcc().setPeriphEnabled(mTim);

    if (mTimers[timerNumber-1])
        THROW(Exception::ResourceBusy); // ALARM!! this timer already in use!

    mTimers[timerNumber-1] = this;

    for (int i=0; i<8; i++)
        mEnabledIrq[i] = false;

    // инициализация тут:
    uint32_t tmp = mTim->CR1;
    if (hasCapability(UpDown))
        tmp = tmp & ~(TIM_CR1_DIR | TIM_CR1_CMS) | 0; // counter mode up
    if (hasCapability(InputOutput))
        tmp = tmp & ~TIM_CR1_CKD | 0; // clock division = 0
    mTim->CR1 = tmp;

    setFrequency(frequency_Hz);

    if (hasCapability(Repetition))
        mTim->RCR = 0; // reset repetition counter
    generateUpdateEvent();
}
//---------------------------------------------------------------------------

HardwareTimer::TimerNumber HardwareTimer::getTimerByPin(Gpio::Config pinConfig)
{
    int num = GpioConfigGetPeriphNumber(pinConfig);
#if defined(STM32G4)
    if (GpioConfigGetPeriphChannel(pinConfig) & 0x40) // TIMx >= TIM16
        num += 16;    
#endif
    return static_cast<TimerNumber>(num);
}

HardwareTimer::ChannelNumber HardwareTimer::getChannelByPin(Gpio::Config pinConfig)
{
    int ch = GpioConfigGetPeriphChannel(pinConfig);
    ch = (ch & 7) - 1;
    return (ChannelNumber)(1 << (ch * 4));
}

void HardwareTimer::selectInputTrigger(InputTrigger trgi)
{
    mTim->SMCR = (mTim->SMCR & ~TIM_SMCR_TS) | ((trgi<<4) & TIM_SMCR_TS);
}

void HardwareTimer::setSlaveMode(SlaveMode sms)
{
    mTim->SMCR = (mTim->SMCR & ~TIM_SMCR_SMS) | (sms & TIM_SMCR_SMS);
}

void HardwareTimer::selectOutputTrigger(TrgSource source)
{
    mTim->CR2 = mTim->CR2 & ~TIM_CR2_MMS | (uint16_t)source;
}

void HardwareTimer::setFrequency(int frequency_Hz)
{
    unsigned int period = 0;
    unsigned int psc = 0;
    if (frequency_Hz)
    {
        period = mInputClk / frequency_Hz;
        psc = period >> 16;
    }
    period = (period / (psc + 1)) - 1;
    mTim->ARR = period;
    if (mTim->CNT >= period)
        mTim->CNT = 0;
    mTim->PSC = psc;
}

int HardwareTimer::frequency() const
{
    int psc = mTim->PSC;
    int arr = mTim->ARR;
    return mInputClk / ((arr + 1) * (psc + 1));
}

int HardwareTimer::clockFrequency() const
{
    int psc = mTim->PSC;
    return mInputClk / (psc + 1);
}
//---------------------------------------------------------------------------

bool HardwareTimer::isReady() const
{
    if (mTim->SR & TIM_SR_UIF)
    {
        mTim->SR = ~TIM_SR_UIF;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

void HardwareTimer::setEnabled(bool enable)
{
    if (enable)
        mTim->CR1 |= TIM_CR1_CEN;
    else
        mTim->CR1 &= ~TIM_CR1_CEN;
}

void HardwareTimer::start()
{
    setEnabled(true);
}

void HardwareTimer::stop()
{
    setEnabled(false);
}

void HardwareTimer::setOnePulseMode(bool enabled)
{
    if (enabled)
        mTim->CR1 |= TIM_CR1_OPM;
    else
        mTim->CR1 &= ~TIM_CR1_OPM;
}
//---------------------------------------------------------------------------

void HardwareTimer::setCompareValue(ChannelNumber ch, unsigned int value)
{
    if (ch & Ch1)
        mTim->CCR1 = value;
    if (ch & Ch2)
        mTim->CCR2 = value;
    if (ch & Ch3)
        mTim->CCR3 = value;
    if (ch & Ch4)
        mTim->CCR4 = value;
}
//---------------------------------------------------------------------------

void HardwareTimer::setCaptureEvent(ChannelNumber ch, NotifyEvent event)
{
    if (ch & Ch1)
        setCC1Event(event);
    if (ch & Ch2)
        setCC2Event(event);
    if (ch & Ch3)
        setCC3Event(event);
    if (ch & Ch4)
        setCC4Event(event);
}

void HardwareTimer::configCapture(ChannelNumber ch, Polarity polarity)
{
    uint16_t chmask = ch;
    for (int i=0; i<4; i++, chmask >>= 4)
    {
        if (!(chmask & 1))
            continue;

        __IO uint32_t &CCR = (&mTim->CCR1)[i];
        __IO uint32_t &CCMR = (&mTim->CCMR1)[i>>1];

        int ccr_shift = i * 4;
        int ccmr_shift = (i & 1) * 8;

        uint8_t filter = 0;//15;

        CCMR &= ~(0xFF << ccmr_shift);
        CCMR |= (TIM_CCMR1_CC1S_0 << ccmr_shift) | (filter << (4 + ccmr_shift)); // channel configuret as input, ICn mapped on TIn

        CCR = 0;
        mTim->ARR = -1;

        // input capture become active after this init
        mTim->CCER = mTim->CCER & ~(0xF << ccr_shift);
        mTim->CCER |= (polarity | 0x0) << ccr_shift; // 0x1 = capture enabled
    }
}

void HardwareTimer::configCapture(Gpio::Config pin, Polarity polarity, NotifyEvent event)
{
    Gpio::config(pin);
    ChannelNumber chan = getChannelByPin(pin);
    configCapture(chan, polarity);
    if (event)
        setCaptureEvent(chan, event);
//    return chan;
}

unsigned int HardwareTimer::captureValue(ChannelNumber ch) const
{
    switch (ch)
    {
        case Ch1: return mTim->CCR1;
        case Ch2: return mTim->CCR2;
        case Ch3: return mTim->CCR3;
        case Ch4: return mTim->CCR4;
    }
    return 0;
}

void HardwareTimer::configPwm(ChannelNumber ch, PwmMode pwmMode, bool inverted)
{
    uint16_t chmask = ch;
    for (int i=0; i<4; i++, chmask >>= 4)
    {
        if (!(chmask & 1))
            continue;

        __IO uint32_t &CCR = (&mTim->CCR1)[i];
        __IO uint32_t &CCMR = (&mTim->CCMR1)[i>>1];

        int cr2_shift = i * 2;
        int ccr_shift = i * 4;
        int ccmr_shift = (i & 1) * 8;

        // outputs are disabled after this init
        mTim->CCER = mTim->CCER & ~(0xF << ccr_shift);
        if (inverted)
            mTim->CCER |= (inverted? 0xA: 0x0) << ccr_shift;

        // preload enable HARDCODED!!
        CCMR = CCMR & ~((TIM_CCMR1_OC1M | TIM_CCMR1_CC1S | TIM_CCMR1_OC1PE) << ccmr_shift)
                    | ((pwmMode | TIM_CCMR1_OC1PE) << ccmr_shift);

        if (hasCapability(Complementary))
        {
            // Reset the Output Compare and Output Compare N IDLE State
            mTim->CR2 = mTim->CR2 & ~(0x0300 << cr2_shift);
        }

        CCR = 0;
    }
}

void HardwareTimer::setChannelEnabled(ChannelNumber ch, bool enabled)
{
    uint16_t mask = (uint16_t)ch & 0x1111;
    if (enabled)
        mTim->CCER |= mask;
    else
        mTim->CCER &= ~mask;
//    mTim->EGR = 1; // generate update event
}

void HardwareTimer::setChannelInverted(ChannelNumber ch, bool inverted)
{
    uint16_t mask = ((uint16_t)ch & 0x1111) << 1;
    if (inverted)
        mTim->CCER |= mask;
    else
        mTim->CCER &= ~mask;
//    mTim->EGR = 1; // generate update event
}

void HardwareTimer::setComplementaryChannelEnabled(ChannelNumber ch, bool enabled)
{
    uint16_t mask = ((uint16_t)ch & 0x0111) << 2;
    if (enabled)
        mTim->CCER |= mask;
    else
        mTim->CCER &= ~mask;
//    mTim->EGR = 1; // generate update event
}

void HardwareTimer::setComplementaryChannelInverted(ChannelNumber ch, bool inverted)
{
    uint16_t mask = ((uint16_t)ch & 0x0111) << 3;
    if (inverted)
        mTim->CCER |= mask;
    else
        mTim->CCER &= ~mask;
//    mTim->EGR = 1; // generate update event
}
//---------------------------------------------------------------------------

void HardwareTimer::enableInterrupt(InterruptSource source)
{
    mEnabledIrq[source] = true;

    if (mTim == TIM1)
    {
        switch (source)
        {
          case isrcUpdate: mIrq = TIM_IRQn(1_UP); break;
          case isrcCC1: case isrcCC2: case isrcCC3: case isrcCC4: mIrq = TIM_IRQn(1_CC); break;
          case isrcCom: case isrcTrigger: mIrq = TIM_IRQn(1_TRG_COM); break;
          case isrcBreak: mIrq = TIM_IRQn(1_BRK); break;
        }
    }
#if defined(TIM8)
    else if (mTim == TIM8)
    {
        switch (source)
        {
          case isrcUpdate: mIrq = TIM_IRQn(8_UP); break;
          case isrcCC1: case isrcCC2: case isrcCC3: case isrcCC4: mIrq = TIM_IRQn(8_CC); break;
          case isrcCom: case isrcTrigger: mIrq = TIM_IRQn(8_TRG_COM); break;
          case isrcBreak: mIrq = TIM_IRQn(8_BRK); break;
        }
    }
#endif

#if defined(TIM20)
    else if (mTim == TIM20)
    {
        switch (source)
        {
          case isrcUpdate: mIrq = TIM_IRQn(20_UP); break;
          case isrcCC1: case isrcCC2: case isrcCC3: case isrcCC4: mIrq = TIM_IRQn(20_CC); break;
          case isrcCom: case isrcTrigger: mIrq = TIM_IRQn(20_TRG_COM); break;
          case isrcBreak: mIrq = TIM_IRQn(20_BRK); break;
        }
    }
#endif
    
//#warning priority for PWM generation must be 0!!!
    NVIC_SetPriority(mIrq, 0);
    NVIC_EnableIRQ(mIrq);

    mTim->DIER |= (1 << source);
}

void HardwareTimer::handleInterrupt()
{
    for (int i=0; i<8; i++)
    {
        if (mEnabledIrq[i])
        {
            uint16_t flag = (1 << i);
#pragma diag_suppress=Pa082
            if (mTim->DIER & mTim->SR & flag)
            {
                mTim->SR = ~flag;
                emitEvent[i]();
            }
        }
    }
}
//---------------------------------------------------------------------------

#ifdef __cplusplus
 extern "C" {
#endif

#define TIM_SIMPLEX_IRQ_HANDLER(x) \
    void TIM_IRQHandler(x)() \
    { \
    if (HardwareTimer::mTimers[x-1]) \
        HardwareTimer::mTimers[x-1]->handleInterrupt(); \
    }

#define TIM_COMPLEX_IRQ_HANDLER(tim, t1, t2) \
    void TIM_IRQHandler(tim)() \
    { \
    if (HardwareTimer::mTimers[t1-1]) \
        HardwareTimer::mTimers[t1-1]->handleInterrupt(); \
    if (t2 && HardwareTimer::mTimers[t2-1]) \
        HardwareTimer::mTimers[t2-1]->handleInterrupt(); \
    }

#if defined(STM32F4)
#define FOREACH_SIMPLEX_TIM_IRQ(f) \
    f(2) f(3) f(4) f(5) f(6) f(7)
#define FOREACH_COMPLEX_TIM_IRQ(f) \
    f(1_BRK, 1, 9)  f(1_UP, 1, 10) f(1_TRG_COM, 1, 11) f(1_CC, 1, 0) \
    f(8_BRK, 8, 12) f(8_UP, 8, 13) f(8_TRG_COM, 8, 14) f(8_CC, 8, 0)

#elif defined(STM32L4) || defined(STM32G4) || defined(STM32F3)
#define FOREACH_SIMPLEX_TIM_IRQ(f) \
    f(2) f(3) f(4) f(5) f(6) f(7) f(9) f(10) f(11) f(12) f(13) f(14)
#define FOREACH_COMPLEX_TIM_IRQ(f) \
    f(1_BRK, 1, 15) f(1_UP, 1, 16) f(1_TRG_COM, 1, 17) f(1_CC, 1, 0) \
    f(8_BRK, 8, 0)  f(8_UP, 8, 0)  f(8_TRG_COM, 8, 0)  f(8_CC, 8, 0) \
    f(20_BRK, 20, 0) f(20_UP, 20, 0) f(20_TRG_COM, 20, 0)  f(20_CC, 20, 0)    
        
#endif

// definition of simplex timer handlers (one irq handler per timer)
FOREACH_SIMPLEX_TIM_IRQ(TIM_SIMPLEX_IRQ_HANDLER);
// definition of complex timer handlers (one irq handler for two timers)
#pragma diag_suppress=Pe175
FOREACH_COMPLEX_TIM_IRQ(TIM_COMPLEX_IRQ_HANDLER);

#ifdef __cplusplus
}
#endif