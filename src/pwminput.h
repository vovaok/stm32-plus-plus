#ifndef _PWMINPUT_H
#define _PWMINPUT_H

#include "hardwaretimer.h"

class PwmInput : private HardwareTimer
{
public:
    PwmInput(Gpio::Config inputPin);
    void setMinFrequency(unsigned int freq);
    
    using HardwareTimer::setEnabled;
    
#if __FPU_PRESENT
    float read();             // call this function to do sample
    float dutyCycle() const;  // duty cycle in range [0 ... 1]
    float percent()   const;  // duty cycle in range [0 ... 100]
    float period_ms() const;  // period, ms
    float frequency() const;  // override for measured frequency, Hz
#else
    int read();
    int dutyCycle() const;  // integer duty cycle in range [0 ... 65536]
    int percent()   const;  // duty cycle in range [0 ... 100]
    int period_ms() const;  // period, ms
    int period_us() const;  // period, us
    int frequency() const;  // measured frequency, Hz
#endif
    
    int rawDutyCycle() const {return m_dutyCycle;}
    int rawPeriod() const {return m_period;}
    
private:
    uint32_t m_dutyCycle;
    uint32_t m_period;
};

#endif