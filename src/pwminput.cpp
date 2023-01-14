#include "pwminput.h"

PwmInput::PwmInput(TimerNumber timerNumber, int channel, Gpio::Config inputPin) :
    HardwareTimer(timerNumber)
{
    switch (timerNumber)
    {
        case 6: case 7: case 10: case 11: case 13: case 14:
            THROW(Exception::InvalidPeriph);
    }
    
    Gpio::config(inputPin);
    
    TIM_SelectInputTrigger(tim(), TIM_TS_TI1FP1); 
    TIM_SelectSlaveMode(tim(), TIM_SlaveMode_Reset); 
    //TIM_PrescalerConfig(tim(), 1, TIM_PSCReloadMode_Immediate);
    
    TIM_ICInitTypeDef ic;
    switch (channel)
    {     
        case 1: ic.TIM_Channel = TIM_Channel_1; break;
        case 2: ic.TIM_Channel = TIM_Channel_2; break;
//        case 3: ic.TIM_Channel = TIM_Channel_3; break;
//        case 4: ic.TIM_Channel = TIM_Channel_4; break;
        default: THROW(Exception::InvalidPeriph);
    }
    ic.TIM_ICFilter = 2;
    ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_PWMIConfig(tim(), &ic);
    
    setCounter(0);
    setCompare1(0);
    setCompare2(0);
}

void PwmInput::setMinFrequency(unsigned int freq)
{
    setPrescaler((inputClk() / freq) >> 16);
}

unsigned int PwmInput::dutyCycle()
{
    unsigned long width = tim()->CCR2;
    unsigned long period = tim()->CCR1;
    if (!period)
        return 0;
    return (width << 16) / period;
}

unsigned int PwmInput::period()
{
    return tim()->CCR1;
}