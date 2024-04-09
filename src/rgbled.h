#ifndef _RGBLED_H
#define _RGBLED_H

#include "pwmout.h"

class RgbLed
{
private:
    PwmOutput *pwm;
    HardwareTimer::ChannelNumber chR, chG, chB;
    unsigned char mR, mG, mB;
    
public:
    RgbLed(Gpio::Config pinR, Gpio::Config pinG, Gpio::Config pinB, int pwmFrequency=1000, bool inverted=false);
    void setR(int value);
    void setG(int value);
    void setB(int value);
    void setRgb(unsigned char r, unsigned char g, unsigned char b);
    
    unsigned char r() const {return mR;}
    unsigned char g() const {return mG;}
    unsigned char b() const {return mB;}
    
    void showBlack();
    void showRed();
    void showGreen();
    void showYellow();
    void showBlue();
    void showMagenta();
    void showCyan();
    void showWhite();
    
    void showOrange();
};

#endif
