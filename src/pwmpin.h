#pragma once

#include "pwmout.h"

class PwmPin
{
    typedef HardwareTimer::PwmMode PwmMode;
    
public:
    PwmPin(Gpio::Config pin);
    
    PwmOutput *device() const {return m_pwm;}
    
    void setInverted(bool inverted);
    void setMode(PwmMode mode);
    void setRawDutyCycle(int value); // 0...65536
    void setDutyCycle(float percent);
    
    void setEnabled(bool enabled);
    void on() {setEnabled(true);}
    void off() {setEnabled(false);}
    bool isEnabled() const {return m_pwm->isChannelEnabled(m_pin);}

private:
    PwmOutput *m_pwm = nullptr;
    Gpio::Config m_pin = Gpio::NoConfig;
};
