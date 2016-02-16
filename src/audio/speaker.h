#ifndef _SPEAKER_H
#define _SPEAKER_H

#include "aspeaker.h"
#include "../dac.h"
#include "../led.h"

class Speaker : public AbstractSpeaker
{
private:
    Dac *mDac;
    //Led *mMutePin;
    Dac::Channels mChannelConfig;
    
    bool mOpened;
  
public:
    Speaker(const AudioFormat &format=AudioFormat(), int interfaceNumber=0);
    
    void open();
    void close();
    
    void mute(bool muting=true);
    bool isMuted();
};

#endif