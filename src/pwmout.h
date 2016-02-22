#ifndef _PWMOUT_H
#define _PWMOUT_H

#include "hardtimer.h"
#include "gpio.h"

class PwmOutput : public HardwareTimer
{
private:
    int mPeriod;
    unsigned long mChMask;
  
public:
    PwmOutput(TimerNumber timerNo, unsigned long pwmFreq);
    
    using HardwareTimer::setEnabled;
    
    void configChannel(ChannelNumber chnum, Gpio::Config pin, Gpio::Config complementaryPin = Gpio::NoConfig);
    void setChannelEnabled(ChannelNumber chnum, bool enabled, bool complementaryEnabled = false);
    void setAllChannelsEnabled(bool enabled);
    void setDutyCycle(ChannelNumber channel, int dutyCycle); // in 16-bit range
    void setDutyCycle(int dutyCycle1, int dutyCycle2 = 0, int dutyCycle3 = 0, int dutyCycle4 = 0);
    
    int maxPwm() const {return mPeriod;}
};

#endif