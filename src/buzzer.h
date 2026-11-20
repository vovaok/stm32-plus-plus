#ifndef _BUZZER_H
#define _BUZZER_H

#include "pwmpin.h"

/// @note Notes are deprecated, use Sequencer instead!

class Buzzer : public PwmPin
{
private:
    int m_time = 0;
    bool m_endFlag = false;
    
    void task();
    void tick(int dt);
  
public:
    Buzzer(Gpio::Config pin);
    
//    void setVolume(unsigned char volume_percent);
    void setFrequency(int f_Hz);
    int currentFrequency() {return device()->frequency();}
    
    void beep(int duration_ms);
    void stop();
    bool isActive() const {return m_time;}
    
    NotifyEvent onBeepDone;
};

#endif