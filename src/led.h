#ifndef _LED_H
#define _LED_H

#include "core/core.h"
#include "stm32_conf.h"
#include "gpio.h"

/*! @file led.h
    @brief GPIO LED control
    */

class Led : private Gpio
{
private:
    int mCountMig;
    bool mInverted;
    
public:
    Led(Gpio::PinName pin, bool inverted=false);
    Led(GPIO_TypeDef *gpio, int pin, bool inverted=false);
    Led(char portLetter, int pinNumber);
    
    inline void on() {mInverted? reset(): set();}
    inline void off() {mInverted? set(): reset();}
    inline void toggle() {Gpio::toggle();}
    inline bool state() const {return mInverted ^ read();}
    inline void setState(bool newState) {write(newState ^ mInverted);}
    
    void toggleSkip(int count);
    void toggleSkip(int count1, int count2);
};

#endif