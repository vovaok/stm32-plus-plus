#include "pwmout.h"

PwmOutput *PwmOutput::mInstances[14] = {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L};

PwmOutput *PwmOutput::instance(TimerNumber timerNo)
{
    if (timerNo < 14)
    {
        if (mInstances[timerNo])
            return mInstances[timerNo];
        return new PwmOutput(timerNo, 10 _kHz);
    }
    return 0L;
}

PwmOutput::PwmOutput(TimerNumber timerNo, unsigned long pwmFreq) :
    HardwareTimer(timerNo, pwmFreq*2), // *2 for center-aligned mode
    m_chMask(0)
{  
    mInstances[timerNo] = this;
    init();
}

PwmOutput::PwmOutput(Gpio::Config pin1, Gpio::Config pin2, Gpio::Config pin3,
                     Gpio::Config pin4, Gpio::Config pin5, Gpio::Config pin6) :
    HardwareTimer((TimerNumber)GpioConfigGetPeriphNumber(pin1)),
    m_chMask(0)
{
    mInstances[(TimerNumber)GpioConfigGetPeriphNumber(pin1)] = this;
    
    if (pin2 != Gpio::NoConfig && GpioConfigGetPeriphNumber(pin2) != GpioConfigGetPeriphNumber(pin1))
        THROW(Exception::InvalidPin);
    if (pin3 != Gpio::NoConfig && GpioConfigGetPeriphNumber(pin3) != GpioConfigGetPeriphNumber(pin1))
        THROW(Exception::InvalidPin);
    if (pin4 != Gpio::NoConfig && GpioConfigGetPeriphNumber(pin4) != GpioConfigGetPeriphNumber(pin1))
        THROW(Exception::InvalidPin);
    if (pin5 != Gpio::NoConfig && GpioConfigGetPeriphNumber(pin5) != GpioConfigGetPeriphNumber(pin1))
        THROW(Exception::InvalidPin);
    if (pin6 != Gpio::NoConfig && GpioConfigGetPeriphNumber(pin6) != GpioConfigGetPeriphNumber(pin1))
        THROW(Exception::InvalidPin);
  
    init();
    configChannel(pin1);
    configChannel(pin2);
    configChannel(pin3);
    configChannel(pin4);
    configChannel(pin5);
    configChannel(pin6);
}

void PwmOutput::init()
{  
    tim()->CR2 |= TIM_CR2_CCPC; // enable preload control
    mPeriod = autoReloadRegister() + 1;
//    setEnabled(true);
    tim()->CR1 |= TIM_CR1_CMS_0; // center aligned mode 1
//    tim()->RCR = 0x01; // update event freq / 2;
    
    if (tim() == TIM1 || tim() == TIM8)
    {
        tim()->BDTR &= 0xFF;
        tim()->BDTR |= 100;
        tim()->BDTR |= TIM_BDTR_MOE | TIM_BDTR_AOE;
#warning TODO
//        TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
//        TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//        TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//        TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
//        TIM_BDTRInitStructure.TIM_DeadTime = 100;
//        TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
//        TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
//        TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//        TIM_BDTRConfig(tim(), &TIM_BDTRInitStructure);
    }
}

void PwmOutput::setDeadtime(float deadtime_us)
{
    int dt = lrintf(deadtime_us * inputClk() * 1e-6f);
    int dtg = dt;
    if (dtg > 127)
        dtg = 64 + dt / 2;
    if (dtg > 191)
        dtg = 160 + dt / 8;
    if (dtg > 223)
        dtg = 192 + dt / 16;
    if (dtg > 255)
        THROW(Exception::OutOfRange);
    else
        tim()->BDTR = (tim()->BDTR & ~0xFF) | dtg;
}

void PwmOutput::configChannel(Gpio::Config pin, Gpio::Config complementaryPin, bool invert)
{
    bool chEnabled = (pin != Gpio::NoConfig);
    bool chnEnabled = (complementaryPin != Gpio::NoConfig);

    int ch = GpioConfigGetPeriphChannel(pin);
    int chn = GpioConfigGetPeriphChannel(complementaryPin);
    if ((chn & 7) != ch)
        THROW(Exception::InvalidPin);
    
    ch = (ch & 7) - 1;
    ChannelNumber channel = (ChannelNumber)(1 << (ch * 4));
    
    Gpio::config(pin);
    Gpio::config(complementaryPin);
  
    configPwm(channel, PwmMode_PWM2, invert);
    
    if (chEnabled)
        m_chMask |= channel;
    if (chnEnabled)
        m_chMask |= ((unsigned long)channel) << 16;
}

void PwmOutput::configChannel(Gpio::Config pin)
{
    if (pin == Gpio::NoConfig)
        return;
    
    int ch = GpioConfigGetPeriphChannel(pin);
    bool comp = ch & 8;
    ch = (ch & 7) - 1;
    ChannelNumber channel = (ChannelNumber)(1 << (ch * 4));
    
    Gpio::config(pin);
    
    configPwm(channel, PwmMode_PWM2);
    
    if (comp)
        m_chMask |= ((unsigned long)channel) << 16;
    else
        m_chMask |= channel;
}

void PwmOutput::configChannelToggleMode(ChannelNumber channel, Gpio::Config pin)
{
    bool chEnabled = (pin != Gpio::NoConfig);
    Gpio::config(pin);
    
    configPwm(channel, PwmMode_Toggle);
    
    if (chEnabled)
        m_chMask |= channel;
}

void PwmOutput::setChannelEnabled(ChannelNumber channel, bool enabled)
{
    HardwareTimer::setChannelEnabled(channel, enabled);
    generateComEvent();
}

void PwmOutput::setChannelEnabled(ChannelNumber channel, bool enabled, bool complementaryEnabled)
{
    HardwareTimer::setChannelEnabled(channel, enabled);
    setComplementaryChannelEnabled(channel, complementaryEnabled);
    generateComEvent();
}

void PwmOutput::setAllChannelsEnabled(bool enabled)
{
    HardwareTimer::setChannelEnabled((ChannelNumber)(m_chMask & 0xFFFF), enabled);
    setComplementaryChannelEnabled((ChannelNumber)(m_chMask >> 16), enabled);
    generateComEvent();
}

void PwmOutput::setDutyCycle(ChannelNumber channel, int dutyCycle)
{
    int pwm = dutyCycle * mPeriod >> 16;
    setCompareValue(channel, pwm);
    generateComEvent();
}

void PwmOutput::setDutyCyclePercent(ChannelNumber channel, float dutyCycle)
{
    setDutyCycle(channel, (int)(BOUND(0.0f, dutyCycle, 100.0f) * 655.36f));
}

void PwmOutput::setDutyCycle(int dutyCycle1, int dutyCycle2, int dutyCycle3, int dutyCycle4)
{
    if (m_chMask & (Ch1 | (Ch1 << 16))) 
        setCompare1(dutyCycle1 * mPeriod >> 16);
    if (m_chMask & (Ch2 | (Ch2 << 16))) 
        setCompare2(dutyCycle2 * mPeriod >> 16);
    if (m_chMask & (Ch3 | (Ch3 << 16))) 
        setCompare3(dutyCycle3 * mPeriod >> 16);
    if (m_chMask & (Ch4 | (Ch4 << 16))) 
        setCompare4(dutyCycle4 * mPeriod >> 16);
    generateComEvent();
}

void PwmOutput::setFrequency(int f_Hz)
{
    HardwareTimer::setFrequency(f_Hz);
    mPeriod = autoReloadRegister() + 1;
}

void PwmOutput::setPwmMode(uint8_t mode)
{
    tim()->CR1 = tim()->CR1 & ~TIM_CR1_CMS | (mode & TIM_CR1_CMS);
}
