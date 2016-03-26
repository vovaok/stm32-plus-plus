#include "rgbled.h"

RgbLed::RgbLed(Gpio::Config pinR, Gpio::Config pinG, Gpio::Config pinB, int pwmFrequency) :
    mR(0), mG(0), mB(0)
{
    TimerNumber tim = HardwareTimer::getTimerByPin(pinR);
    chR = HardwareTimer::getChannelByPin(pinR);
    chG = HardwareTimer::getChannelByPin(pinG);
    chB = HardwareTimer::getChannelByPin(pinB);
    
    if (tim == TimNone)
        throw Exception::invalidPeriph;
    if (chR == ChNone || chG == ChNone || chB == ChNone)
        throw Exception::invalidPin;
  
    pwm = new PwmOutput(tim, 1_kHz);
    pwm->configChannel(chR, pinR); 
    pwm->configChannel(chG, pinG);
    pwm->configChannel(chB, pinB);

    for (int i=0; i<30000; i++);
    pwm->setAllChannelsEnabled(true);
}

void RgbLed::setR(unsigned char value)
{
    mR = value;
    pwm->setDutyCycle(chR, value<<8);
}

void RgbLed::setG(unsigned char value)
{
    mG = value;
    pwm->setDutyCycle(chG, value<<8);
}

void RgbLed::setB(unsigned char value)
{
    mB = value;
    pwm->setDutyCycle(chB, value<<8);
}

void RgbLed::setRgb(unsigned char r, unsigned char g, unsigned char b)
{
    mR = r;
    mG = g;
    mB = b;
    pwm->setDutyCycle(chR, r<<8);
    pwm->setDutyCycle(chG, g<<8);
    pwm->setDutyCycle(chB, b<<8);
}
//---------------------------------------------------------------------------

void RgbLed::showBlack()
{
    setRgb(0, 0, 0);
}

void RgbLed::showRed()
{
    setRgb(255, 0, 0);
}

void RgbLed::showGreen()
{
    setRgb(0, 255, 0);
}

void RgbLed::showYellow()
{
    setRgb(255, 255, 0);
}

void RgbLed::showBlue()
{
    setRgb(0, 0, 255);
}

void RgbLed::showMagenta()
{
    setRgb(255, 0, 255);
}

void RgbLed::showCyan()
{
    setRgb(0, 255, 255);
}

void RgbLed::showWhite()
{
    setRgb(255, 255, 255);
}

void RgbLed::showOrange()
{
    setRgb(255, 64, 0);
}