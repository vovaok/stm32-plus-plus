#ifndef _LED7_H
#define _LED7_H

#include "gpio.h"
#include "core/timer.h"
#include <stdlib.h>

class Led7 
{
private:
   Timer *mTimer;
   Gpio *mDataPort;
   Gpio *mControlPort;
   ByteArray mBuffer;
   unsigned char mDigitCount;
   unsigned char mControlStartPin;
   unsigned char mDataStartPin;
   unsigned long mShiftCnt;
   
   static const unsigned char font[256];
   
   void onTimer();
  
public:
    Led7(int digitCount, Gpio::PinName dataFirstPin, Gpio::PinName controlFirstPin);
    ~Led7();

    void setBuffer(const char *buffer, char dot=0);
    void setInt(int value, char dot=0);
    
    void write(int value);
    void write(unsigned short value);
    void write(unsigned char value);
    void write(float value);
    void write(const char *s);
};

#endif // DELTA_H
