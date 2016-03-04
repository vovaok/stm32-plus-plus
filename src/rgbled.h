#ifndef _RGBLED_H
#define _RGBLED_H

#include "pwmout.h"

class RgbLed
{
private:
    PwmOutput *pwm;
    ChannelNumber chR, chG, chB;
    
public:
    RgbLed(Gpio::Config pinR, Gpio::Config pinG, Gpio::Config pinB, int pwmFrequency=1_kHz);
    void setR(unsigned char value);
    void setG(unsigned char value);
    void setB(unsigned char value);
    void setRgb(unsigned char r, unsigned char g, unsigned char b);
    
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