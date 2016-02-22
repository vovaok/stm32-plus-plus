#include "rgbled.h"

RgbLed::RgbLed(Gpio::Config pinR, Gpio::Config pinG, Gpio::Config pinB, int pwmFrequency)
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
    pwm->setDutyCycle(chR, value<<8);
}

void RgbLed::setG(unsigned char value)
{
    pwm->setDutyCycle(chG, value<<8);
}

void RgbLed::setB(unsigned char value)
{
    pwm->setDutyCycle(chB, value<<8);
}

void RgbLed::setRgb(unsigned char r, unsigned char g, unsigned char b)
{
    pwm->setDutyCycle(chR, r<<8);
    pwm->setDutyCycle(chG, g<<8);
    pwm->setDutyCycle(chB, b<<8);
}