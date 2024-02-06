#pragma once

#include "gpio.h"
#include "core/coretypes.h"

class RotaryEncoder
{
public:
    RotaryEncoder(Gpio::PinName pinA, Gpio::PinName pinB);
    
    Closure<void(int)> onStep;
    NotifyEvent onStepUp;
    NotifyEvent onStepDown;
    
private:
    Gpio *m_pinA;
    Gpio *m_pinB;
    
    bool m_a, m_b;
    
    void task();
};
