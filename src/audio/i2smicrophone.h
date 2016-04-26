#ifndef _I2SMICROPHONE_H
#define _I2SMICROPHONE_H

#include "amicrophone.h"
#include "i2s.h"

class I2SMicrophone : public AbstractMicrophone
{
private:
    I2S *mDev;
    bool mOpened;
  
public:
    I2SMicrophone(const AudioFormat &format, I2S *device);
  
    void open();
    void close();
};

#endif
