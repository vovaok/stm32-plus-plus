#ifndef _ABSTRACT_SPEAKER_H
#define _ABSTRACT_SPEAKER_H

#include "audiodev.h"

// this is interface
class AbstractSpeaker : public AudioDevice
{    
//protected:
//    AudioFormat mFormat;
//    Dma *mDma;
//    HardwareTimer *mTimer;
//    
//    unsigned char* buffer() const {return mBuffer;}
  
public:
    AbstractSpeaker(const AudioFormat &format = AudioFormat());
//    virtual ~AbstractSpeaker();
    
    virtual void mute(bool muting=true) {};
    virtual bool isMuted() {return false;}
    virtual void setVolume(int value) {};
};

#endif