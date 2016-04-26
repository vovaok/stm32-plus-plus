#ifndef _BUZZER_H
#define _BUZZER_H

#include "pwmout.h"

class Buzzer
{
private:
    PwmOutput *mPwm;
    ChannelNumber mChan;
    int mTime;
    bool mEndFlag;
    
    static unsigned short mNoteFreqs[12];
    static int getFreqByNote(unsigned char note);
    
    void task();
    void tick(int dt);
  
public:
    Buzzer(Gpio::Config pin);
    
//    void setVolume(unsigned char volume_percent);
    void setFrequency(int f_Hz);
    void setEnabled(bool enabled);
    void on() {setEnabled(true);}
    void off() {setEnabled(false);}
    void beep(int duration_ms);
    void beep(unsigned char note, int duration_ms);
    
    NotifyEvent onBeepDone;
};

#endif