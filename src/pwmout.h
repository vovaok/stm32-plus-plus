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
    
    // maybe npuroguTcq
//    enum Channel
//    {
//        NoChannel = 0,
//        Ch1 = HardwareTimer::Ch1,
//        Ch2 = HardwareTimer::Ch2,
//        Ch3 = HardwareTimer::Ch3,
//        Ch4 = HardwareTimer::Ch4,
//        Ch1n = Ch1 << 16;
//        Ch2n = Ch2 << 16;
//        Ch3n = Ch3 << 16;
//        Ch4n = Ch4 << 16;
//    };
    
    void configChannel(Gpio::Config pin, Gpio::Config complementaryPin, bool inverted = false);
    void configChannel(Gpio::Config pin, bool inverted = false);
    
//    [[deprecated]] void configChannel(ChannelNumber ch, PwmMode pwmMode=PwmMode_PWM2, bool inverted=false)
//        { HardwareTimer::configPwm(ch, pwmMode, inverted); }
    using HardwareTimer::configPwm;
    inline void configPwm(Gpio::Config pin, PwmMode mode, bool inverted = false)
        { HardwareTimer::configPwm(getChannelByPin(pin), mode, inverted); }
      
    inline void configChannelToggleMode(ChannelNumber channel)
        { configPwm(channel, PwmMode_Toggle); }
    
    inline void configChannelToggleMode(Gpio::Config pin)
        { configPwm(pin, PwmMode_Toggle); }
    
    void setChannelEnabled(Gpio::Config pin, bool enabled);
    bool isChannelEnabled(Gpio::Config pin);
    void setChannelInverted(Gpio::Config pin, bool inverted);
    bool isChannelInverted(Gpio::Config pin);
    void setChannelEnabled(ChannelNumber channel, bool enabled);
    void setChannelEnabled(ChannelNumber channel, bool enabled, bool complementaryEnabled);
    void setAllChannelsInverted(bool inverted);
    void setAllChannelsEnabled(bool enabled);
    void setDutyCycle(ChannelNumber channel, int value); // in 16-bit range
    void setDutyCycle(Gpio::Config pin, int value); // in 16-bit range
    void setDutyCyclePercent(ChannelNumber channel, float value);
    void setDutyCyclePercent(Gpio::Config pin, float value);
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