#ifndef _PWMINPUT_H
#define _PWMINPUT_H

#include "hardtimer.h"

class PwmInput : private HardwareTimer
{
private:
    
public:
    PwmInput(TimerNumber timerNumber, int channel, Gpio::Config inputPin);
    void setMinFrequency(unsigned int freq);
    
    using HardwareTimer::setEnabled;
    
    unsigned int dutyCycle();
    unsigned int period();
};

#endif