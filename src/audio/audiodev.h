#ifndef _AUDIODEV_H
#define _AUDIODEV_H

#include <string.h>
#include "../hardtimer.h"
#include "../dma.h"
#include "audioformat.h"

class AudioDevice
{
public:
    typedef enum {Mono=1, Stereo=2} ChannelConfig;
    
private:   
    unsigned char *mBuffer;
    int mBufferSize;
    
protected:
    AudioFormat mFormat;
    Dma *mDma;
    HardwareTimer *mTimer;
    
    unsigned char* buffer() const {return mBuffer;}
  
public:
    AudioDevice(const AudioFormat &format = AudioFormat());
    virtual ~AudioDevice();
    
    const AudioFormat &format() const {return mFormat;}
    
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void setBufferSize(int size);
    int bufferSize() const {return mBufferSize;}
    
    //virtual void writeSample(int sample);
    virtual int read(void *buffer, int size);
    virtual int write(void *buffer, int size);
    
    virtual void pause(bool pauseOn);
    virtual void stop();
};

#endif
