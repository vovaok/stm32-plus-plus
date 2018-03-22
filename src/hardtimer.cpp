#include "hardtimer.h"

HardwareTimer* HardwareTimer::mTimers[14] = {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L};

HardwareTimer::HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz) :
  mEnabled(false)
{
    int clkDiv = 1;
    switch (timerNumber)
    {
#if !defined(STM32F37X)
      case 1:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        mTim = TIM1;
#warning TIM1_IRQ not implemented
//        mIrq = TIM1_IRQn;
        clkDiv = 0;
        break;
#endif
        
      case 2:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        mTim = TIM2;
        mIrq = TIM2_IRQn;
        break;
        
      case 3:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        mTim = TIM3;
        mIrq = TIM3_IRQn;
        break;
        
      case 4:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        mTim = TIM4;
        mIrq = TIM4_IRQn;
        break;
        
      case 5:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
        mTim = TIM5;
        mIrq = TIM5_IRQn;
        break;
        
      case 6:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
        mTim = TIM6;
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
        break;
        
#if !defined(STM32F37X)        
      case 8:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
        mTim = TIM8;
#warning TIM8_IRQ not implemented
//        mIrq = TIM8_IRQn;
        clkDiv = 0;
        break;
        
      case 9:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
        mTim = TIM9;
        mIrq = TIM1_BRK_TIM9_IRQn;
        clkDiv = 0;
        break;
        
      case 10:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
        mTim = TIM10;
        mIrq = TIM1_UP_TIM10_IRQn;
        clkDiv = 0;
        break;
        
      case 11:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
        mTim = TIM11;
        mIrq = TIM1_TRG_COM_TIM11_IRQn;
        clkDiv = 0;
        break;
#endif
        
      case 12:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
        mTim = TIM12;
#if !defined(STM32F37X)
        mIrq = TIM8_BRK_TIM12_IRQn;
#else
        mIrq = TIM12_IRQn;
#endif
        break;
        
      case 13:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
        mTim = TIM13;
#if !defined(STM32F37X)        
        mIrq = TIM8_UP_TIM13_IRQn;
#else
        mIrq = TIM13_IRQn;
#endif
        break;
        
      case 14:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
        mTim = TIM14;
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
    mInputClk = Rcc::sysClk() >> clkDiv;
    
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
    #if defined(STM32F37X)
  
    return static_cast<TimerNumber>(GpioConfigGetPeriphNumber(pinConfig));
  
    #else
  
    switch (pinConfig)
    {      
        // TIM1
        case Gpio::TIM1_BKIN_PA6: case Gpio::TIM1_CH1N_PA7: case Gpio::TIM1_CH1_PA8: case Gpio::TIM1_CH2_PA9:
        case Gpio::TIM1_CH3_PA10: case Gpio::TIM1_CH4_PA11: case Gpio::TIM1_ETR_PA12: case Gpio::TIM1_CH2N_PB0:
        case Gpio::TIM1_CH3N_PB1: case Gpio::TIM1_BKIN_PB12: case Gpio::TIM1_CH1N_PB13: case Gpio::TIM1_CH2N_PB14:
        case Gpio::TIM1_CH3N_PB15: case Gpio::TIM1_ETR_PE7: case Gpio::TIM1_CH1N_PE8: case Gpio::TIM1_CH1_PE9:
        case Gpio::TIM1_CH2N_PE10: case Gpio::TIM1_CH2_PE11: case Gpio::TIM1_CH3N_PE12: case Gpio::TIM1_CH3_PE13:
        case Gpio::TIM1_CH4_PE14: case Gpio::TIM1_BKIN_PE15: return Tim1;
        // TIM2   
        case Gpio::TIM2_CH1_PA0: case Gpio::TIM2_CH2_PA1: case Gpio::TIM2_CH3_PA2: case Gpio::TIM2_CH4_PA3:
        case Gpio::TIM2_CH1_PA5: case Gpio::TIM2_CH1_PA15: case Gpio::TIM2_CH2_PB3: case Gpio::TIM2_CH3_PB10:
        case Gpio::TIM2_CH4_PB11: /*case Gpio::TIM2_ETR_PA0: case Gpio::TIM2_ETR_PA5: case Gpio::TIM2_ETR_PA15:*/
        return Tim2;
        // TIM3
        case Gpio::TIM3_CH1_PA6: case Gpio::TIM3_CH2_PA7: case Gpio::TIM3_CH3_PB0: case Gpio::TIM3_CH4_PB1:
        case Gpio::TIM3_CH1_PB4: case Gpio::TIM3_CH2_PB5: case Gpio::TIM3_CH1_PC6: case Gpio::TIM3_CH2_PC7:
        case Gpio::TIM3_CH3_PC8: case Gpio::TIM3_CH4_PC9: case Gpio::TIM3_ETR: return Tim3;
        // TIM4
        case Gpio::TIM4_CH1_PB6: case Gpio::TIM4_CH2_PB7: case Gpio::TIM4_CH3_PB8: case Gpio::TIM4_CH4_PB9:
        case Gpio::TIM4_CH1_PD12: case Gpio::TIM4_CH2_PD13: case Gpio::TIM4_CH3_PD14: case Gpio::TIM4_CH4_PD15:
        case Gpio::TIM4_ETR: return Tim4;
        // TIM5
        case Gpio::TIM5_CH1_PA0: case Gpio::TIM5_CH2_PA1: case Gpio::TIM5_CH3_PA2: case Gpio::TIM5_CH4_PA3:
        case Gpio::TIM5_CH1_PH10: case Gpio::TIM5_CH2_PH11: case Gpio::TIM5_CH3_PH12: case Gpio::TIM5_CH4_PI0:
        return Tim5;
        // TIM8
        case Gpio::TIM8_ETR_PA0: case Gpio::TIM8_CH1N_PA5: case Gpio::TIM8_BKIN_PA6: case Gpio::TIM8_CH1N_PA7:
        case Gpio::TIM8_CH2N_PB0: case Gpio::TIM8_CH3N_PB1: case Gpio::TIM8_CH2N_PB14: case Gpio::TIM8_CH3N_PB15:
        case Gpio::TIM8_CH1_PC6: case Gpio::TIM8_CH2_PC7: case Gpio::TIM8_CH3_PC8: case Gpio::TIM8_CH4_PC9:
        case Gpio::TIM8_CH1N_PH13: case Gpio::TIM8_CH2N_PH14: case Gpio::TIM8_CH3N_PH15: case Gpio::TIM8_CH4_PI2:
        case Gpio::TIM8_ETR_PI3: case Gpio::TIM8_BKIN_PI4: case Gpio::TIM8_CH1_PI5: case Gpio::TIM8_CH2_PI6:
        case Gpio::TIM8_CH3_PI7: return Tim8;
        // TIM9
        case Gpio::TIM9_CH1_PA2: case Gpio::TIM9_CH2_PA3: case Gpio::TIM9_CH1_PE5: case Gpio::TIM9_CH2_PE6:
        return Tim9;
        // TIM10
        case Gpio::TIM10_CH1_PB8: case Gpio::TIM10_CH1_PF6: return Tim10;
        // TIM11
        case Gpio::TIM11_CH1_PB9: case Gpio::TIM11_CH1_PF7: return Tim11;
        // TIM12
        case Gpio::TIM12_CH1_PB14: case Gpio::TIM12_CH2_PB15: case Gpio::TIM12_CH1_PH6: case Gpio::TIM12_CH2_PH9:
        return Tim12;
        // TIM13
        case Gpio::TIM13_CH1_PA6: case Gpio::TIM13_CH1_PF8: return Tim13;
        // TIM14
        case Gpio::TIM14_CH1_PA7: case Gpio::TIM14_CH1_PF9: return Tim14; 
        // NO TIMER
        default: return TimNone;
    }
    
    #endif
}

ChannelNumber HardwareTimer::getChannelByPin(Gpio::Config pinConfig)
{
    #if defined(STM32F37X)

    switch (GpioConfigGetPeriphChannel(pinConfig))
    {
        case 1: return Ch1;
        case 2: return Ch2;
        case 3: return Ch3;
        case 4: return Ch4;
        default: return ChNone;
    }
  
    #else    
    
    switch (pinConfig)
    {
    
        // CH1
        case Gpio::TIM1_CH1N_PA7: case Gpio::TIM1_CH1_PA8: case Gpio::TIM1_CH1N_PB13: case Gpio::TIM1_CH1N_PE8:
        case Gpio::TIM1_CH1_PE9: case Gpio::TIM2_CH1_PA0: case Gpio::TIM2_CH1_PA5: case Gpio::TIM2_CH1_PA15: 
        case Gpio::TIM3_CH1_PA6: case Gpio::TIM3_CH1_PB4: case Gpio::TIM3_CH1_PC6: case Gpio::TIM4_CH1_PB6: 
        case Gpio::TIM4_CH1_PD12: case Gpio::TIM5_CH1_PA0: case Gpio::TIM5_CH1_PH10: case Gpio::TIM8_CH1N_PA5:
        case Gpio::TIM8_CH1N_PA7: case Gpio::TIM8_CH1_PC6: case Gpio::TIM8_CH1N_PH13: case Gpio::TIM8_CH1_PI5:
        case Gpio::TIM9_CH1_PA2: case Gpio::TIM9_CH1_PE5: case Gpio::TIM10_CH1_PB8: case Gpio::TIM10_CH1_PF6:
        case Gpio::TIM11_CH1_PB9: case Gpio::TIM11_CH1_PF7: case Gpio::TIM12_CH1_PB14: case Gpio::TIM12_CH1_PH6:
        case Gpio::TIM13_CH1_PA6: case Gpio::TIM13_CH1_PF8: case Gpio::TIM14_CH1_PA7: case Gpio::TIM14_CH1_PF9:
        return Ch1;
        // CH2
        case Gpio::TIM1_CH2_PA9: case Gpio::TIM1_CH2N_PB0: case Gpio::TIM1_CH2N_PB14: case Gpio::TIM1_CH2N_PE10:
        case Gpio::TIM1_CH2_PE11: case Gpio::TIM2_CH2_PA1: case Gpio::TIM2_CH2_PB3: case Gpio::TIM3_CH2_PA7: 
        case Gpio::TIM3_CH2_PB5: case Gpio::TIM3_CH2_PC7: case Gpio::TIM4_CH2_PB7: case Gpio::TIM4_CH2_PD13:
        case Gpio::TIM5_CH2_PA1: case Gpio::TIM5_CH2_PH11: case Gpio::TIM8_CH2N_PB0: case Gpio::TIM8_CH2N_PB14: 
        case Gpio::TIM8_CH2_PC7: case Gpio::TIM8_CH2N_PH14: case Gpio::TIM8_CH2_PI6: case Gpio::TIM9_CH2_PA3:
        case Gpio::TIM9_CH2_PE6: case Gpio::TIM12_CH2_PB15: case Gpio::TIM12_CH2_PH9: return Ch2;        
        // CH3
        case Gpio::TIM1_CH3_PA10: case Gpio::TIM1_CH3N_PB1: case Gpio::TIM1_CH3N_PB15: case Gpio::TIM1_CH3N_PE12:
        case Gpio::TIM1_CH3_PE13: case Gpio::TIM2_CH3_PA2: case Gpio::TIM2_CH3_PB10: case Gpio::TIM3_CH3_PB0:
        case Gpio::TIM3_CH3_PC8: case Gpio::TIM4_CH3_PB8: case Gpio::TIM4_CH3_PD14: case Gpio::TIM5_CH3_PA2:
        case Gpio::TIM5_CH3_PH12: case Gpio::TIM8_CH3N_PB1: case Gpio::TIM8_CH3N_PB15: case Gpio::TIM8_CH3_PC8:
        case Gpio::TIM8_CH3N_PH15: case Gpio::TIM8_CH3_PI7: return Ch3;        
        // CH4
        case Gpio::TIM1_CH4_PA11: case Gpio::TIM1_CH4_PE14: case Gpio::TIM2_CH4_PA3: case Gpio::TIM2_CH4_PB11:
        case Gpio::TIM3_CH4_PB1: case Gpio::TIM3_CH4_PC9: case Gpio::TIM4_CH4_PB9: case Gpio::TIM4_CH4_PD15:
        case Gpio::TIM5_CH4_PA3: case Gpio::TIM5_CH4_PI0: case Gpio::TIM8_CH4_PC9: case Gpio::TIM8_CH4_PI2:
        return Ch4;
        // NO CHANNEL
        default: return ChNone;
    }
    
    #endif
}
//---------------------------------------------------------------------------

void HardwareTimer::selectInputTrigger(InputTrigger trgi)
{
    mTim->SMCR = (mTim->SMCR & ~TIM_SMCR_TS) | (trgi & TIM_SMCR_TS);
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
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
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
        case Ch1: channel = TIM_Channel_1; break;
        case Ch2: channel = TIM_Channel_2; break;
        case Ch3: channel = TIM_Channel_3; break;
        case Ch4: channel = TIM_Channel_4; break;
        default: return;
    }
    TIM_CCxCmd(tim(), channel, enabled? TIM_CCx_Enable: TIM_CCx_Disable);
//    TIM_GenerateEvent(tim(), TIM_EventSource_COM); 
}
//---------------------------------------------------------------------------

void HardwareTimer::enableInterrupt(InterruptSource source)
{
    mEnabledIrq[source] = true;
  
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

//void TIM1_IRQHandler()
//{
//    if (HardwareTimer::mTimers[1-1])
//        HardwareTimer::mTimers[1-1]->handleInterrupt();
//}
   
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

//void TIM8_IRQHandler()
//{
//    if (HardwareTimer::mTimers[8-1])
//        HardwareTimer::mTimers[8-1]->handleInterrupt();
//}

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

#ifdef __cplusplus
}
#endif