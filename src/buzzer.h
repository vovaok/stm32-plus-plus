#ifndef _BUZZER_H
#define _BUZZER_H

#include "pwmout.h"

class Buzzer
{
public:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char note;
        unsigned short duration_ms;
        unsigned char _reserved;
    } Note;
#pragma pack(pop)
  
private:
    PwmOutput *mPwm;
    HardwareTimer::ChannelNumber mChan;
    int mTime;
    bool mEndFlag;
    bool m_isPlaying;
    
    Note *mPlayBuf;
    int mPlaySize;
    int mPlayCnt;
    
    static unsigned short mNoteFreqs[12];
    
    void task();
    void tick(int dt);
    
public:
    static int getFreqByNote(unsigned char note);
  
public:
    Buzzer(Gpio::Config pin);
    
//    void setVolume(unsigned char volume_percent);
    void setFrequency(int f_Hz);
    void setEnabled(bool enabled);
    void on() {setEnabled(true);}
    void off() {setEnabled(false);}
    void beep(int duration_ms);
    void beep(unsigned char note, int duration_ms);
    void beep(Note note);
    
    int currentFrequency() {return mPwm->frequency();}
    bool isPlaying() const {return m_isPlaying;}
    
    void play(Note *buffer, int count);
    void stop();
    
    PwmOutput *device() {return mPwm;}
    
    NotifyEvent onBeepDone;
    NotifyEvent onPlayDone;
};

#endif