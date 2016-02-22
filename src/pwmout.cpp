#include "pwmout.h"

PwmOutput::PwmOutput(TimerNumber timerNo, unsigned long pwmFreq) :
    HardwareTimer(timerNo, pwmFreq),
    mChMask(0)
{  
    TIM_CCPreloadControl(tim(), ENABLE);
    mPeriod = autoReloadRegister() + 1;
    setEnabled(true);
    
    if (timerNo == Tim1 || timerNo == Tim8)
    {
        TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
        TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
        TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
        TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
        TIM_BDTRInitStructure.TIM_DeadTime = 5;
        TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
        TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
        TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
        TIM_BDTRConfig(tim(), &TIM_BDTRInitStructure);
    }
}

void PwmOutput::configChannel(ChannelNumber chnum, Gpio::Config pin, Gpio::Config complementaryPin)
{
    bool chEnabled = (pin != Gpio::NoConfig);
    bool chnEnabled = (complementaryPin != Gpio::NoConfig);
  
    Gpio::config(pin);
    Gpio::config(complementaryPin);
  
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;//(chEnabled)? TIM_OutputState_Enable: TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//(chnEnabled)? TIM_OutputNState_Enable: TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

    switch (chnum)
    {
        case Ch1: TIM_OC1Init(tim(), &TIM_OCInitStructure); TIM_OC1PreloadConfig(tim(), TIM_OCPreload_Enable); break;
        case Ch2: TIM_OC2Init(tim(), &TIM_OCInitStructure); TIM_OC2PreloadConfig(tim(), TIM_OCPreload_Enable); break;
        case Ch3: TIM_OC3Init(tim(), &TIM_OCInitStructure); TIM_OC3PreloadConfig(tim(), TIM_OCPreload_Enable); break;
        case Ch4: TIM_OC4Init(tim(), &TIM_OCInitStructure); TIM_OC4PreloadConfig(tim(), TIM_OCPreload_Enable); break;
    }
    
    if (chEnabled)
        mChMask |= chnum;
    if (chnEnabled)
        mChMask |= ((unsigned long)chnum) << 16;
}

void PwmOutput::setChannelEnabled(ChannelNumber chnum, bool enabled, bool complementaryEnabled)
{
    uint16_t channel = 0;
    switch (chnum)
    {
        case Ch1: channel = TIM_Channel_1; break;
        case Ch2: channel = TIM_Channel_2; break;
        case Ch3: channel = TIM_Channel_3; break;
        case Ch4: channel = TIM_Channel_4; break;
        default: return;
    }
    TIM_CCxCmd(tim(), channel, enabled? TIM_CCx_Enable: TIM_CCx_Disable);
    TIM_CCxNCmd(tim(), channel, complementaryEnabled? TIM_CCxN_Enable: TIM_CCxN_Disable);
    TIM_GenerateEvent(tim(), TIM_EventSource_COM); 
}

void PwmOutput::setAllChannelsEnabled(bool enabled)
{
    if (mChMask & Ch1)
        TIM_CCxCmd(tim(), TIM_Channel_1, enabled? TIM_CCx_Enable: TIM_CCx_Disable);
    if (mChMask & (Ch1 << 16))
        TIM_CCxNCmd(tim(), TIM_Channel_1, enabled? TIM_CCxN_Enable: TIM_CCxN_Disable);
    
    if (mChMask & Ch2)
        TIM_CCxCmd(tim(), TIM_Channel_2, enabled? TIM_CCx_Enable: TIM_CCx_Disable);
    if (mChMask & (Ch2 << 16))
        TIM_CCxNCmd(tim(), TIM_Channel_2, enabled? TIM_CCxN_Enable: TIM_CCxN_Disable);
    
    if (mChMask & Ch3)
        TIM_CCxCmd(tim(), TIM_Channel_3, enabled? TIM_CCx_Enable: TIM_CCx_Disable);
    if (mChMask & (Ch3 << 16))
        TIM_CCxNCmd(tim(), TIM_Channel_3, enabled? TIM_CCxN_Enable: TIM_CCxN_Disable);
    
    if (mChMask & Ch4)
        TIM_CCxCmd(tim(), TIM_Channel_4, enabled? TIM_CCx_Enable: TIM_CCx_Disable);
    if (mChMask & (Ch4 << 16))
        TIM_CCxNCmd(tim(), TIM_Channel_4, enabled? TIM_CCxN_Enable: TIM_CCxN_Disable);

    TIM_GenerateEvent(tim(), TIM_EventSource_COM); 
}

void PwmOutput::setDutyCycle(ChannelNumber channel, int dutyCycle)
{
    int pwm = dutyCycle * mPeriod >> 16;
    switch (channel)
    {
        case Ch1: setCompare1(pwm); break;
        case Ch2: setCompare2(pwm); break;
        case Ch3: setCompare3(pwm); break;
        case Ch4: setCompare4(pwm); break;
    };
    TIM_GenerateEvent(tim(), TIM_EventSource_COM); 
}

void PwmOutput::setDutyCycle(int dutyCycle1, int dutyCycle2, int dutyCycle3, int dutyCycle4)
{
    if (mChMask & (Ch1 | (Ch1 << 16))) 
        setCompare1(dutyCycle1 * mPeriod >> 16);
    if (mChMask & (Ch2 | (Ch2 << 16))) 
        setCompare2(dutyCycle2 * mPeriod >> 16);
    if (mChMask & (Ch3 | (Ch3 << 16))) 
        setCompare3(dutyCycle3 * mPeriod >> 16);
    if (mChMask & (Ch4 | (Ch4 << 16))) 
        setCompare4(dutyCycle4 * mPeriod >> 16);
    TIM_GenerateEvent(tim(), TIM_EventSource_COM);
}
