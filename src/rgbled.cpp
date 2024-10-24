#include "rgbled.h"

RgbLed::RgbLed(Gpio::Config pinR, Gpio::Config pinG, Gpio::Config pinB, int pwmFrequency, bool inverted) :
    mR(0), mG(0), mB(0)
{
    HardwareTimer::TimerNumber tim = HardwareTimer::getTimerByPin(pinR);
    if (tim == HardwareTimer::TimNone)
        tim = HardwareTimer::getTimerByPin(pinG);
    if (tim == HardwareTimer::TimNone)
        tim = HardwareTimer::getTimerByPin(pinB);
    
    if (tim == HardwareTimer::TimNone)
        THROW(Exception::InvalidPeriph);
    
    chR = HardwareTimer::getChannelByPin(pinR);
    chG = HardwareTimer::getChannelByPin(pinG);
    chB = HardwareTimer::getChannelByPin(pinB);
    
//    if (chR == ChNone || chG == ChNone || chB == ChNone)
//        throw Exception::invalidPin;
  
    pwm = new PwmOutput(tim, 1000);
    if (chR != HardwareTimer::ChNone)
        pwm->configChannel(pinR, Gpio::NoConfig, inverted); 
    if (chG != HardwareTimer::ChNone)
        pwm->configChannel(pinG, Gpio::NoConfig, inverted);
    if (chB != HardwareTimer::ChNone)
        pwm->configChannel(pinB, Gpio::NoConfig, inverted);

    //for (int i=0; i<30000; i++);
    pwm->setAllChannelsEnabled(true);
}

void RgbLed::setR(int value)
{
    mR = BOUND(0, value, 255);
    if (chR != HardwareTimer::ChNone)
        pwm->setDutyCycle(chR, value<<8);
}

void RgbLed::setG(int value)
{
    mG = BOUND(0, value, 255);
    if (chG != HardwareTimer::ChNone)
        pwm->setDutyCycle(chG, value<<8);
}

void RgbLed::setB(int value)
{
    mB = BOUND(0, value, 255);
    if (chB != HardwareTimer::ChNone)
        pwm->setDutyCycle(chB, value<<8);
}

void RgbLed::setRgb(unsigned char r, unsigned char g, unsigned char b)
{
    mR = r;
    mG = g;
    mB = b;
    if (chR != HardwareTimer::ChNone)
        pwm->setDutyCycle(chR, r<<8);
    if (chG != HardwareTimer::ChNone)
        pwm->setDutyCycle(chG, g<<8);
    if (chB != HardwareTimer::ChNone)
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
