#include "hardtimer.h"

HardwareTimer* HardwareTimer::mTimers[14] = {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L};

HardwareTimer::HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz) :
  mEnabled(false)
{
    int clkDiv = 1;
    switch (timerNumber)
    {
      case 1:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        mTim = TIM1;
#warning TIM1_IRQ not implemented
//        mIrq = TIM1_IRQn;
        clkDiv = 0;
        break;
        
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
        mIrq = TIM6_DAC_IRQn;
        break;
        
      case 7:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
        mTim = TIM7;
        mIrq = TIM7_IRQn;
        break;
        
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
        
      case 12:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
        mTim = TIM12;
        mIrq = TIM8_BRK_TIM12_IRQn;
        break;
        
      case 13:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
        mTim = TIM13;
        mIrq = TIM8_UP_TIM13_IRQn;
        break;
        
      case 14:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
        mTim = TIM14;
        mIrq = TIM8_TRG_COM_TIM14_IRQn;
        break;
        
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
    setFrequency(frequency_Hz);
}
//---------------------------------------------------------------------------

TimerNumber HardwareTimer::getTimerByPin(Gpio::Config pinConfig)
{
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
}

ChannelNumber HardwareTimer::getChannelByPin(Gpio::Config pinConfig)
{
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
}
//---------------------------------------------------------------------------

void HardwareTimer::selectOutputTrigger(TrgSource source)
{
    TIM_SelectOutputTrigger(mTim, source);
}

void HardwareTimer::setFrequency(int frequency_Hz)
{
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
    TIM_Cmd(mTim, enable? ENABLE: DISABLE);
}

void HardwareTimer::start()
{
    setEnabled(true); 
}

void HardwareTimer::stop()
{
    setEnabled(false);
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
    GPIOD->BSRRL = 1<<0;
    if (HardwareTimer::mTimers[5-1])
        HardwareTimer::mTimers[5-1]->handleInterrupt();
    GPIOD->BSRRH = 1<<0;
}

void TIM6_IRQHandler()
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