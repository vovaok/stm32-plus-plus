#ifndef _LED_H
#define _LED_H

#include "core/core.h"
#include "core/timer.h"
#include "gpio.h"

/*! @file led.h
    @brief GPIO LED control
    */

class Led : private Gpio
{
public:
    Led(Gpio::PinName pin, bool inverted=false);
    Led(GPIO_TypeDef *gpio, int pin, bool inverted=false);
    Led(char portLetter, int pinNumber); // DEPRECATED
    virtual ~Led();

    inline void on() {setState(true);}
    inline void off() {setState(false);}
    inline bool state() const {return mInverted ^ read();}
    void setState(bool newState);

    using Gpio::toggle;

    void setBlinkInterval(int value);
    bool isBlinking() const;
    void setBlinkingEnabled(bool enabled);
    void blink();
    void blinkOnce();

    void toggleSkip(int count);
    void toggleSkip(int count1, int count2);

private:
    int mCountMig;
    bool mInverted;
    Timer *m_blinkTimer = nullptr;

    Timer *timer();
};

#endif