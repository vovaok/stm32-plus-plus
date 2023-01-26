#include "hardwaretimer.h"

HardwareTimer* HardwareTimer::mTimers[19] = {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L};

HardwareTimer::HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz) :
  mEnabled(false)
{
    unsigned int hclk = Rcc::hClk();
    unsigned int pclk1 = Rcc::pClk1();
    unsigned int pclk2 = Rcc::pClk2();
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
        mIrq = TIM1_UP_TIM10_IRQn;
        break;
#endif
        
      case 2:
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        mTim = TIM2;
        mIrq = TIM2_IRQn;
        mInputClk = pclk1;
        break;
        
      case 3:
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
        mTim = TIM3;
        mIrq = TIM3_IRQn;
        mInputClk = pclk1;
        break;
        
      case 4:
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
        mTim = TIM4;
        mIrq = TIM4_IRQn;
        mInputClk = pclk1;
        break;
        
      case 5:
        RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
        mTim = TIM5;
        mIrq = TIM5_IRQn;
        mInputClk = pclk1;
        break;
        
      case 6:
        RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
        mTim = TIM6;
        mInputClk = pclk1;
#if !defined(STM32F37X)
        mIrq = TIM6_DAC_IRQn;
#else
        mIrq = TIM6_DAC1_IRQn;
#endif
        break;
        
      case 7:
        RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
        mTim = TIM7;
        mIrq = TIM7_IRQn;
        mInputClk = pclk1;
        break;
        
#if !defined(STM32F37X)        
      case 8:
        RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
        mTim = TIM8;
        mIrq = TIM8_UP_TIM13_IRQn;
        break;
        
      case 9:
        RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
        mTim = TIM9;
        mIrq = TIM1_BRK_TIM9_IRQn;
        break;
        
      case 10:
        RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
        mTim = TIM10;
        mIrq = TIM1_UP_TIM10_IRQn;
        mInputClk = pclk1; // or pclk2 ? 
        break;
        
      case 11:
        RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
        mTim = TIM11;
        mIrq = TIM1_TRG_COM_TIM11_IRQn;
        break;
#endif
        
      case 12:
        RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
        mTim = TIM12;
        mInputClk = pclk1;
#if !defined(STM32F37X)
        mIrq = TIM8_BRK_TIM12_IRQn;
#else
        mIrq = TIM12_IRQn;
#endif
        break;
        
      case 13:
        RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
        mTim = TIM13;
        mInputClk = pclk1;
#if !defined(STM32F37X)        
        mIrq = TIM8_UP_TIM13_IRQn;
#else
        mIrq = TIM13_IRQn;
#endif
        break;
        
      case 14:
        RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
        mTim = TIM14;
        mInputClk = pclk1;
#if !defined(STM32F37X)
        mIrq = TIM8_TRG_COM_TIM14_IRQn;
#else
        mIrq = TIM14_IRQn;
#endif
        break;
        
#if defined(STM32F37X)
      case 15:
        RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
        mTim = TIM15;
        mIrq = TIM15_IRQn;
        break;
        
      case 16:
        RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
        mTim = TIM16;
        mIrq = TIM16_IRQn;
        break;
        
      case 17:
        RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
        mTim = TIM17;
        mIrq = TIM17_IRQn;
        break;
        
      case 18:
        RCC->APB1ENR |= RCC_APB1ENR_TIM18EN;
        mTim = TIM18;
        mIrq = TIM18_DAC2_IRQn;
        mInputClk = pclk1;
        break;
        
      case 19:
        RCC->APB2ENR |= RCC_APB2ENR_TIM19EN;
        mTim = TIM19;
        mIrq = TIM19_IRQn;
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
    mTim->ARR = (period / (psc + 1)) - 1;
    if (mTim->CNT >= mTim->ARR)
        mTim->CNT = 0;
    mTim->PSC = psc;
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

void HardwareTimer::setComplementaryChannelEnabled(ChannelNumber ch, bool enabled)
{
    uint16_t mask = ((uint16_t)ch & 0x0111) << 2;
    if (enabled)
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
          case isrcUpdate: mIrq = TIM1_UP_TIM10_IRQn; break;
          case isrcCC1: case isrcCC2: case isrcCC3: case isrcCC4: mIrq = TIM1_CC_IRQn; break;
          case isrcCom: case isrcTrigger: mIrq = TIM1_TRG_COM_TIM11_IRQn; break;
          case isrcBreak: mIrq = TIM1_BRK_TIM9_IRQn; break;
        }
    }
    else if (mTim == TIM8)
    {
        switch (source)
        {
          case isrcUpdate: mIrq = TIM8_UP_TIM13_IRQn; break;
          case isrcCC1: case isrcCC2: case isrcCC3: case isrcCC4: mIrq = TIM8_CC_IRQn; break;
          case isrcCom: case isrcTrigger: mIrq = TIM8_TRG_COM_TIM14_IRQn; break;
          case isrcBreak: mIrq = TIM8_BRK_TIM12_IRQn; break;
        }
    }
    
//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = mIrq;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
    
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
            if (mTim->SR & flag)
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

void TIM1_BRK_TIM9_IRQHandler()
{
    if (HardwareTimer::mTimers[1-1])
        HardwareTimer::mTimers[1-1]->handleInterrupt();
}

void TIM1_UP_TIM10_IRQHandler()
{
    if (HardwareTimer::mTimers[1-1])
        HardwareTimer::mTimers[1-1]->handleInterrupt();
}

void TIM1_TRG_COM_TIM11_IRQHandler()
{
    if (HardwareTimer::mTimers[1-1])
        HardwareTimer::mTimers[1-1]->handleInterrupt();
}

void TIM1_CC_IRQHandler()
{
    if (HardwareTimer::mTimers[1-1])
        HardwareTimer::mTimers[1-1]->handleInterrupt();
}
   
void TIM2_IRQHandler()
{
    if (HardwareTimer::mTimers[2-1])
        HardwareTimer::mTimers[2-1]->handleInterrupt();
}

void TIM3_IRQHandler()
{
    if (HardwareTimer::mTimers[3-1])
        HardwareTimer::mTimers[3-1]->handleInterrupt();
}

void TIM4_IRQHandler()
{
    if (HardwareTimer::mTimers[4-1])
        HardwareTimer::mTimers[4-1]->handleInterrupt();
}

void TIM5_IRQHandler()
{
    if (HardwareTimer::mTimers[5-1])
        HardwareTimer::mTimers[5-1]->handleInterrupt();
}

void TIM6_DAC_IRQHandler()
{
    if (HardwareTimer::mTimers[6-1])
        HardwareTimer::mTimers[6-1]->handleInterrupt();
}

void TIM7_IRQHandler()
{
    if (HardwareTimer::mTimers[7-1])
        HardwareTimer::mTimers[7-1]->handleInterrupt();
}

void TIM8_BRK_TIM12_IRQHandler()
{
    if (HardwareTimer::mTimers[8-1])
        HardwareTimer::mTimers[8-1]->handleInterrupt();
}

void TIM8_UP_TIM13_IRQHandler()
{
    if (HardwareTimer::mTimers[8-1])
        HardwareTimer::mTimers[8-1]->handleInterrupt();
}

void TIM8_TRG_COM_TIM14_IRQHandler()
{
    if (HardwareTimer::mTimers[8-1])
        HardwareTimer::mTimers[8-1]->handleInterrupt();
}

void TIM8_CC_IRQHandler()
{
    if (HardwareTimer::mTimers[8-1])
        HardwareTimer::mTimers[8-1]->handleInterrupt();
}

void TIM9_IRQHandler()
{
    if (HardwareTimer::mTimers[9-1])
        HardwareTimer::mTimers[9-1]->handleInterrupt();
}

void TIM10_IRQHandler()
{
    if (HardwareTimer::mTimers[10-1])
        HardwareTimer::mTimers[10-1]->handleInterrupt();
}

void TIM11_IRQHandler()
{
    if (HardwareTimer::mTimers[11-1])
        HardwareTimer::mTimers[11-1]->handleInterrupt();
}

void TIM12_IRQHandler()
{
    if (HardwareTimer::mTimers[12-1])
        HardwareTimer::mTimers[12-1]->handleInterrupt();
}

void TIM13_IRQHandler()
{
    if (HardwareTimer::mTimers[13-1])
        HardwareTimer::mTimers[13-1]->handleInterrupt();
}

void TIM14_IRQHandler()
{
    if (HardwareTimer::mTimers[14-1])
        HardwareTimer::mTimers[14-1]->handleInterrupt();
}

void TIM15_IRQHandler()
{
    if (HardwareTimer::mTimers[15-1])
        HardwareTimer::mTimers[15-1]->handleInterrupt();
}

#ifdef __cplusplus
}
#endif