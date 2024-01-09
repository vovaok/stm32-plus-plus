#include "hardwaretimer.h"

HardwareTimer* HardwareTimer::mTimers[19] = {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L};

HardwareTimer::HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz) :
  mEnabled(false)
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
#if !defined(STM32F37X)
      case 1:
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
        mTim = TIM1;
        mIrq = TIM_IRQn(1_UP); // у этого таймера 4 прерывания, задаётся позже, когда нужно
        break;
#endif
        
      case 2:
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        mTim = TIM2;
        mIrq = TIM_IRQn(2);
        mInputClk = pclk1;
        break;
        
      case 3:
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
        mTim = TIM3;
        mIrq = TIM_IRQn(3);
        mInputClk = pclk1;
        break;
        
      case 4:
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
        mTim = TIM4;
        mIrq = TIM_IRQn(4);
        mInputClk = pclk1;
        break;
        
      case 5:
        RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
        mTim = TIM5;
        mIrq = TIM_IRQn(5);
        mInputClk = pclk1;
        break;
        
      case 6:
        RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
        mTim = TIM6;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(6);
        break;
        
      case 7:
        RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
        mTim = TIM7;
        mIrq = TIM_IRQn(7);
        mInputClk = pclk1;
        break;
        
#if defined(TIM8_UP_IRQ)     
      case 8:
        RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
        mTim = TIM8;
        mIrq = TIM_IRQn(8_UP); // у этого таймера 4 прерывания, задаётся позже, когда нужно
        break;
#endif
#if defined(TIM9_IRQ)
      case 9:
        RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
        mTim = TIM9;
        mIrq = TIM_IRQn(9);
        break;
#endif
#if defined(TIM10_IRQ)      
      case 10:
        RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
        mTim = TIM10;
        mIrq = TIM_IRQn(10);
        mInputClk = pclk1; // or pclk2 ??? 
        break;
#endif
#if defined(TIM11_IRQ)     
      case 11:
        RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
        mTim = TIM11;
        mIrq = TIM_IRQn(11);
        break;
#endif
#if defined(TIM12_IRQ)        
      case 12:
        RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
        mTim = TIM12;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(12);
        break;
#endif
#if defined(TIM13_IRQ)         
      case 13:
        RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
        mTim = TIM13;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(13);
        break;
#endif
#if defined(TIM14_IRQ)         
      case 14:
        RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
        mTim = TIM14;
        mInputClk = pclk1;
        mIrq = TIM_IRQn(14);
        break;
#endif
#if defined(TIM15_IRQ)         
      case 15:
        RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
        mTim = TIM15;
        mIrq = TIM_IRQn(15);
        break;
#endif
#if defined(TIM16_IRQ)         
      case 16:
        RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
        mTim = TIM16;
        mIrq = TIM_IRQn(16);
        break;
#endif
#if defined(TIM17_IRQ)         
      case 17:
        RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
        mTim = TIM17;
        mIrq = TIM_IRQn(17);
        break;
#endif
#if defined(TIM18_IRQ)         
      case 18:
        RCC->APB1ENR |= RCC_APB1ENR_TIM18EN;
        mTim = TIM18;
        mIrq = TIM_IRQn(18);
        mInputClk = pclk1;
        break;
#endif
#if defined(TIM19_IRQ)         
      case 19:
        RCC->APB2ENR |= RCC_APB2ENR_TIM19EN;
        mTim = TIM19;
        mIrq = TIM_IRQn(19);
        break;
#endif
        
      default:
        return;
    }
    
    if (mTimers[timerNumber-1])
        THROW(Exception::ResourceBusy); // ALARM!! this timer already in use!
    
    mTimers[timerNumber-1] = this;
    
    for (int i=0; i<8; i++)
        mEnabledIrq[i] = false;
    
    // инициализация тут:    
    uint32_t tmp = mTim->CR1;
    if (mTim == TIM1 || mTim == TIM8 || mTim == TIM2 || mTim == TIM3 || mTim == TIM4 || mTim == TIM5)
        tmp = tmp & ~(TIM_CR1_DIR | TIM_CR1_CMS) | 0; // counter mode up
    if (mTim != TIM6 && mTim != TIM7)
        tmp = tmp & ~TIM_CR1_CKD | 0; // clock division = 0
    mTim->CR1 = tmp;
    
    setFrequency(frequency_Hz);
    
    if (mTim == TIM1 || mTim == TIM8)
        mTim->RCR = 0; // repetition counter
    generateUpdateEvent();
}
//---------------------------------------------------------------------------

HardwareTimer::TimerNumber HardwareTimer::getTimerByPin(Gpio::Config pinConfig)
{
    return static_cast<TimerNumber>(GpioConfigGetPeriphNumber(pinConfig));
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
    mEnabled = enable;
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
    
        uint8_t filter = 15;
        
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

        if (mTim == TIM1 || mTim == TIM8)
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
    
    NVIC_SetPriority(mIrq, 2);
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
    
#elif defined(STM32L4) || defined(STM32G4)
#define FOREACH_SIMPLEX_TIM_IRQ(f) \
    f(2) f(3) f(4) f(5) f(6) f(7) f(9) f(10) f(11) f(12) f(13) f(14)
#define FOREACH_COMPLEX_TIM_IRQ(f) \
    f(1_BRK, 1, 15) f(1_UP, 1, 16) f(1_TRG_COM, 1, 17) f(1_CC, 1, 0) \
    f(8_BRK, 8, 0)  f(8_UP, 8, 0)  f(8_TRG_COM, 8, 0)  f(8_CC, 8, 0)       
    
#endif

// definition of simplex timer handlers (one irq handler per timer)
FOREACH_SIMPLEX_TIM_IRQ(TIM_SIMPLEX_IRQ_HANDLER);
// definition of complex timer handlers (one irq handler for two timers)
#pragma diag_suppress=Pe175
FOREACH_COMPLEX_TIM_IRQ(TIM_COMPLEX_IRQ_HANDLER);

#ifdef __cplusplus
}
#endif