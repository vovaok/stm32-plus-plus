#include "pwmout.h"

PwmOutput *PwmOutput::mInstances[14] = {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L};

PwmOutput *PwmOutput::instance(TimerNumber timerNo)
{
    if (timerNo < 14)
    {
        if (mInstances[timerNo])
            return mInstances[timerNo];
        return new PwmOutput(timerNo, 10000);
    }
    return 0L;
}

PwmOutput::PwmOutput(TimerNumber timerNo, int pwmFreq) :
    HardwareTimer(timerNo),
    m_chMask(0)
{
    mInstances[timerNo] = this;
    setFrequency(pwmFreq);
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
    setPwmAlignMode(CenterAlignedMode1);
//    tim()->RCR = 0x01; // update event freq / 2;

    setEnabled(true);
    if (hasCapability(Complementary))
    {
        tim()->BDTR &= 0xFF; //! @todo check this
        tim()->BDTR |= 100;
        tim()->BDTR |= TIM_BDTR_MOE | TIM_BDTR_AOE;
    }
}

void PwmOutput::setDeadtime(float deadtime_us)
{
    int dt = static_cast<int>(deadtime_us * inputClk() * 1e-6f);
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

    configPwm(channel, PwmMode_PWM1, invert);

    if (chEnabled)
        m_chMask |= channel;
    if (chnEnabled)
        m_chMask |= ((unsigned long)channel) << 16;
}

void PwmOutput::configChannel(Gpio::Config pin, bool invert)
{
    if (pin == Gpio::NoConfig)
        return;

    int ch = GpioConfigGetPeriphChannel(pin);
    bool comp = ch & 8;
    ch = (ch & 7) - 1;
    ChannelNumber channel = (ChannelNumber)(1 << (ch * 4));

    Gpio::config(pin);

    configPwm(channel, PwmMode_PWM1, invert);

    if (comp)
        m_chMask |= ((unsigned long)channel) << 16;
    else
        m_chMask |= channel;
}

void PwmOutput::configPwmMode(Gpio::Config pin, PwmMode mode)
{
    ChannelNumber channel = getChannelByPin(pin);
    Gpio::config(pin);
    configPwm(channel, mode);
    m_chMask |= channel;
}

void PwmOutput::configChannelToggleMode(ChannelNumber channel)
{
    configPwm(channel, PwmMode_Toggle);
}

void PwmOutput::configChannelToggleMode(Gpio::Config pin)
{
    configPwmMode(pin, PwmMode_Toggle);
}

void PwmOutput::setChannelInverted(Gpio::Config pin, bool inverted)
{
    ChannelNumber channel = getChannelByPin(pin);
    setChannelInverted(channel, inverted);
}

void PwmOutput::setChannelEnabled(Gpio::Config pin, bool enabled)
{
    ChannelNumber channel = getChannelByPin(pin);
    setChannelEnabled(channel, enabled);
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

void PwmOutput::setDutyCycle(ChannelNumber channel, int value)
{
    int pwm = value * mPeriod >> 16;
    setCompareValue(channel, pwm);
    generateComEvent();
}

void PwmOutput::setDutyCycle(Gpio::Config pin, int value)
{
    ChannelNumber channel = getChannelByPin(pin);
    setDutyCycle(channel, value);
}

void PwmOutput::setDutyCyclePercent(ChannelNumber channel, float value)
{
    setDutyCycle(channel, (int)(BOUND(0.0f, value, 100.0f) * 655.36f));
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
    if (tim()->CR1 & TIM_CR1_CMS_Msk) // if center-aligned mode
        f_Hz <<= 1;
    HardwareTimer::setFrequency(f_Hz);
    mPeriod = autoReloadRegister() + 1;
}

int PwmOutput::frequency()
{
    int f = HardwareTimer::frequency();
    if (tim()->CR1 & TIM_CR1_CMS_Msk) // if center-aligned mode
        return f >> 1;
    return f;
}

void PwmOutput::setPwmAlignMode(PwmAlignMode mode)
{
    int f = frequency();
    tim()->CR1 = tim()->CR1 & ~TIM_CR1_CMS | (mode & TIM_CR1_CMS);
    setFrequency(f);
}

void PwmOutput::setAllChannelsInverted(bool inverted)
{
    setChannelInverted((ChannelNumber)(m_chMask & 0xFFFF), inverted);
    setComplementaryChannelInverted((ChannelNumber)(m_chMask >> 16), inverted);
}
