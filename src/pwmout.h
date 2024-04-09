#ifndef _PWMOUT_H
#define _PWMOUT_H

#include "hardwaretimer.h"

class PwmOutput : public HardwareTimer
{
private:
    int mPeriod;
    uint32_t m_chMask;
    static PwmOutput *mInstances[14];
    
    void init();
  
public:
    PwmOutput(TimerNumber timerNo, int pwmFreq);
    PwmOutput(Gpio::Config pin1,                Gpio::Config pin2=Gpio::NoConfig,
              Gpio::Config pin3=Gpio::NoConfig, Gpio::Config pin4=Gpio::NoConfig,
              Gpio::Config pin5=Gpio::NoConfig, Gpio::Config pin6=Gpio::NoConfig);
    
    static PwmOutput *instance(TimerNumber timerNo);
    
    using HardwareTimer::setEnabled;
    
    void configChannel(Gpio::Config pin, Gpio::Config complementaryPin, bool invert = false);
    void configChannel(Gpio::Config pin, bool invert = false);
    void configPwmMode(Gpio::Config pin, PwmMode mode);
    void configChannelToggleMode(ChannelNumber channel);
    void configChannelToggleMode(Gpio::Config pin);
    void setChannelInverted(Gpio::Config pin, bool inverted);
    using HardwareTimer::setChannelInverted;
    void setChannelEnabled(Gpio::Config pin, bool enabled);
    void setChannelEnabled(ChannelNumber channel, bool enabled);
    void setChannelEnabled(ChannelNumber channel, bool enabled, bool complementaryEnabled);
    void setAllChannelsInverted(bool inverted);
    void setAllChannelsEnabled(bool enabled);
    void setDutyCycle(ChannelNumber channel, int value); // in 16-bit range
    void setDutyCycle(Gpio::Config pin, int value); // in 16-bit range
    void setDutyCyclePercent(ChannelNumber channel, float value);
    void setDutyCycle(int dutyCycle1, int dutyCycle2 = 0, int dutyCycle3 = 0, int dutyCycle4 = 0);
    
    enum PwmAlignMode
    {
        EdgeAlignedMode = 0,
        CenterAlignedMode1 = 1 << 5,
        CenterAlignedMode2 = 2 << 5,
        CenterAlignedMode3 = 3 << 5,
    };
    
    void setPwmAlignMode(PwmAlignMode mode);
    
    void setFrequency(int f_Hz);
    int frequency();
    void setDeadtime(float deadtime_us);
    
    int maxPwm() const {return mPeriod;}
};

#endif