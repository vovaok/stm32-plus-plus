#include "pwmpin.h"

PwmPin::PwmPin(Gpio::Config pin)
    : m_pin(pin)
{
    int tim = GpioConfigGetPeriphNumber(pin);
    m_pwm = PwmOutput::instance((HardwareTimer::TimerNumber)tim);
    
    if (!m_pwm)
        THROW(Exception::InvalidPeriph);
    
    if (m_pwm)
        m_pwm->configChannel(pin);
    
    m_pwm->start();
}

void PwmPin::setInverted(bool inverted)
{
    m_pwm->setChannelInverted(m_pin, inverted);
}

void PwmPin::setMode(PwmMode mode)
{
    m_pwm->configPwm(m_pin, mode, m_pwm->isChannelInverted(m_pin));
}

void PwmPin::setEnabled(bool enabled)
{
    m_pwm->setChannelEnabled(m_pin, enabled);
}

void PwmPin::setRawDutyCycle(int value)
{
    m_pwm->setDutyCycle(m_pin, value);
}

void PwmPin::setDutyCycle(float percent)
{
    m_pwm->setDutyCyclePercent(m_pin, percent);
}
