#ifndef _LED_H
#define _LED_H

#include "core/core.h"
#include "stm32_conf.h"

/*! @file led.h
    @brief GPIO LED control
    */

class Led
{
private:
    GPIO_TypeDef *mPort;
    uint32_t mPin;
    
    bool mState;
    char mCountMig;
    
public:
    Led(char portLetter, int pinNumber);
    
    void on();
    void off();
    void toggle();
    
    bool state() const {return mState;}
    void setState(bool newState);
    void toggleSkip(char count);
    void toggleSkip(char count1, char count2);
};

#endif