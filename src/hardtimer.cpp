#include "hardtimer.h"

HardwareTimer* HardwareTimer::mTimers[19] = {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L};

HardwareTimer::HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz) :
  mEnabled(false)
{
    unsigned int pclk1 = Rcc::pClk1();
    unsigned int pclk2 = Rcc::pClk2();
#if defined(STM32F37X)
    if (pclk1 != Rcc::hClk())
        pclk1 *= 2;
    if (pclk2 != Rcc::hClk())
        pclk2 *= 2;
#endif
    mInputClk = pclk2;
    switch (timerNumber)
    {
#if !defined(STM32F37X)
      case 1:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        mTim = TIM1;
        break;
#endif
        
      case 2:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        mTim = TIM2;
        mIrq = TIM2_IRQn;
        mInputClk = pclk1;
        break;
        
      case 3:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        mTim = TIM3;
        mIrq = TIM3_IRQn;
        mInputClk = pclk1;
        break;
        
      case 4:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        mTim = TIM4;
        mIrq = TIM4_IRQn;
        mInputClk = pclk1;
        break;
        
      case 5:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
        mTim = TIM5;
        mIrq = TIM5_IRQn;
        mInputClk = pclk1;
        break;
        
      case 6:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
        mTim = TIM6;
        mInputClk = pclk1;
#if !defined(STM32F37X)
        mIrq = TIM6_DAC_IRQn;
#else
        mIrq = TIM6_DAC1_IRQn;
#endif
        break;
        
      case 7:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
        mTim = TIM7;
        mIrq = TIM7_IRQn;
        mInputClk = pclk1;
        break;
        
#if !defined(STM32F37X)        
      case 8:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
        mTim = TIM8;
        break;
        
      case 9:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
        mTim = TIM9;
        mIrq = TIM1_BRK_TIM9_IRQn;
        break;
        
      case 10:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
        mTim = TIM10;
        mIrq = TIM1_UP_TIM10_IRQn;
        mInputClk = pclk1; // or pclk2 ? 
        break;
        
      case 11:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
        mTim = TIM11;
        mIrq = TIM1_TRG_COM_TIM11_IRQn;
        break;
#endif
        
      case 12:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
        mTim = TIM12;
        mInputClk = pclk1;
#if !defined(STM32F37X)
        mIrq = TIM8_BRK_TIM12_IRQn;
#else
        mIrq = TIM12_IRQn;
#endif
        break;
        
      case 13:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
        mTim = TIM13;
        mInputClk = pclk1;
#if !defined(STM32F37X)        
        mIrq = TIM8_UP_TIM13_IRQn;
#else
        mIrq = TIM13_IRQn;
#endif
        break;
        
      case 14:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
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
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
        mTim = TIM15;
        mIrq = TIM15_IRQn;
        break;
        
      case 16:  
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
        mTim = TIM16;
        mIrq = TIM16_IRQn;
        break;
        
      case 17:  
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
        mTim = TIM17;
        mIrq = TIM17_IRQn;
        break;
        
      case 18:  
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM18, ENABLE);
        mTim = TIM18;
        mIrq = TIM18_DAC2_IRQn;
        mInputClk = pclk1;
        break;
        
      case 19:  
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM19, ENABLE);
        mTim = TIM19;
        mIrq = TIM19_IRQn;
        break;
#endif
        
      default:
        return;
    }
    
    if (mTimers[timerNumber-1])
        throw Exception::resourceBusy; // ALARM!! this timer already in use!
    
    mTimers[timerNumber-1] = this;
    
    for (int i=0; i<8; i++)
        mEnabledIrq[i] = false;
    
    // deinit на всякий
    TIM_DeInit(mTim);
    
    // инициализация тут:
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    unsigned int period = 0;  
    unsigned int psc = 0;
    if (frequency_Hz)
    {
        period = mInputClk / frequency_Hz;  
        psc = period >> 16;
    }
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = (period / (psc + 1)) - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(mTim, &TIM_TimeBaseStructure);
    
}
//---------------------------------------------------------------------------

TimerNumber HardwareTimer::getTimerByPin(Gpio::Config pinConfig)
{
    return static_cast<TimerNumber>(GpioConfigGetPeriphNumber(pinConfig));
}

ChannelNumber HardwareTimer::getChannelByPin(Gpio::Config pinConfig)
{
    switch (GpioConfigGetPeriphChannel(pinConfig))
    {
        case 1: return Ch1;
        case 2: return Ch2;
        case 3: return Ch3;
        case 4: return Ch4;
        default: return ChNone;
    }
}
//---------------------------------------------------------------------------

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
    TIM_SelectOutputTrigger(mTim, source);
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
    mTim->PSC = psc;
}
//---------------------------------------------------------------------------

bool HardwareTimer::isReady() const
{
    if (TIM_GetFlagStatus(mTim, TIM_FLAG_Update))
    {
        TIM_ClearFlag(mTim, TIM_FLAG_Update);
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
        mTim->CR1 &= (uint16_t)~TIM_CR1_CEN;
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
        mTim->CR1 &= (uint16_t)~TIM_CR1_OPM;
}
//---------------------------------------------------------------------------

void HardwareTimer::setCompareValue(ChannelNumber ch, unsigned int value)
{
    switch (ch)
    {
        case Ch1: mTim->CCR1 = value; break;
        case Ch2: mTim->CCR2 = value; break;
        case Ch3: mTim->CCR3 = value; break;
        case Ch4: mTim->CCR4 = value; break;
    }
}
//---------------------------------------------------------------------------

void HardwareTimer::setCaptureEvent(ChannelNumber ch, NotifyEvent event)
{
    switch (ch)
    {
        case Ch1: setCC1Event(event); break;
        case Ch2: setCC2Event(event); break;
        case Ch3: setCC3Event(event); break;
        case Ch4: setCC4Event(event); break;
    }
}

void HardwareTimer::configCapture(ChannelNumber ch, Polarity pol)
{
    TIM_ICInitTypeDef ic;
    ic.TIM_ICFilter = 2;
    ic.TIM_ICPolarity = TIM_ICPolarity_Falling;
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    switch (ch)
    {
        case Ch1: ic.TIM_Channel = TIM_Channel_1; break;
        case Ch2: ic.TIM_Channel = TIM_Channel_2; break;
        case Ch3: ic.TIM_Channel = TIM_Channel_3; break;
        case Ch4: ic.TIM_Channel = TIM_Channel_4; break;
    }
    TIM_ICInit(tim(), &ic);
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
    return -1;
}

void HardwareTimer::configPWM(ChannelNumber ch, bool inverted)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    
    TIM_OCInitStructure.TIM_OCPolarity = inverted? TIM_OCPolarity_Low: TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = inverted? TIM_OCNPolarity_Low: TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

    switch (ch)
    {
        case Ch1: TIM_OC1Init(tim(), &TIM_OCInitStructure); TIM_OC1PreloadConfig(tim(), TIM_OCPreload_Enable); break;
        case Ch2: TIM_OC2Init(tim(), &TIM_OCInitStructure); TIM_OC2PreloadConfig(tim(), TIM_OCPreload_Enable); break;
        case Ch3: TIM_OC3Init(tim(), &TIM_OCInitStructure); TIM_OC3PreloadConfig(tim(), TIM_OCPreload_Enable); break;
        case Ch4: TIM_OC4Init(tim(), &TIM_OCInitStructure); TIM_OC4PreloadConfig(tim(), TIM_OCPreload_Enable); break;
    }
}

void HardwareTimer::setPWMEnabled(ChannelNumber ch, bool enabled)
{
    uint16_t channel = 0;
    switch (ch)
    {
        case Ch1: channel = 1<<0; break;
        case Ch2: channel = 1<<4; break;
        case Ch3: channel = 1<<8; break;
        case Ch4: channel = 1<<12; break;
        default: return;
    }
    if (enabled)
        mTim->CCER |= channel;
    else
        mTim->CCER &= ~channel;
//    TIM_CCxCmd(tim(), channel, enabled? TIM_CCx_Enable: TIM_CCx_Disable);
//    TIM_GenerateEvent(tim(), TIM_EventSource_COM); 
}

void HardwareTimer::setComplementaryPWMEnabled(ChannelNumber ch, bool enabled)
{
    uint16_t channel = 0;
    switch (ch)
    {
        case Ch1: channel = 1<<2; break;
        case Ch2: channel = 1<<6; break;
        case Ch3: channel = 1<<10; break;
        default: return;
    }
    if (enabled)
        mTim->CCER |= channel;
    else
        mTim->CCER &= ~channel;
//    TIM_GenerateEvent(tim(), TIM_EventSource_COM); 
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
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = mIrq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_ITConfig(mTim, 1<<source, ENABLE);
}

void HardwareTimer::handleInterrupt()
{
    uint16_t source = TIM_IT_Update; // 0x0001
    for (int i=0; i<8; i++, source<<=1) // select next source
    {
        if (mEnabledIrq[i])
        {
            if (mTim->SR & source)  //(TIM_GetITStatus(mTim, source) == SET)
            {
                mTim->SR = ~source; //TIM_ClearITPendingBit(mTim, source);
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