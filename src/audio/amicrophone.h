#ifndef _ABSTRACT_MICROPHONE_H
#define _ABSTRACT_MICROPHONE_H

#include "audiodev.h"

// this is interface
class AbstractMicrophone : public AudioDevice
{    
//protected:
//    AudioFormat mFormat;
//    Dma *mDma;
//    HardwareTimer *mTimer;
//    
//    unsigned char* buffer() const {return mBuffer;}
  
public:
    AbstractMicrophone(const AudioFormat &format = AudioFormat());
//    virtual ~AbstractSpeaker();
    
    virtual void mute(bool muting=true) {};
    virtual void setGain(int value) {};
};

#endif
