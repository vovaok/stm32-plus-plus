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
    
public:
    Led(Gpio::PinName pin);
    Led(char portLetter, int pinNumber);
    
    inline void on() {set();}
    inline void off() {reset();}
    inline void toggle() {Gpio::toggle();}
    inline bool state() const {return read();}
    inline void setState(bool newState) {write(newState);}
    
    void toggleSkip(int count);
    void toggleSkip(int count1, int count2);
};

#endif