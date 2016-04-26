#ifndef _MICROPHONE_H
#define _MICROPHONE_H

#include <stdarg.h>
#include "amicrophone.h"
#include "../adc.h"

class Microphone : public AbstractMicrophone
{
private:
    Adc *mAdc;
    Adc::Channel *mInterfaces;
    
    bool mOpened;
  
public:
    Microphone(const AudioFormat &format=AudioFormat(), int interface1=Adc::VrefInt, ...);
    ~Microphone();
    
    void open();
    void close();
    
    //int read(void *data, int size);
};

#endif
