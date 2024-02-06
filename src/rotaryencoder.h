#pragma once

#include "gpio.h"
#include "core/coretypes.h"

class RotaryEncoder
{
public:
    RotaryEncoder(Gpio::PinName pinA, Gpio::PinName pinB);
    void setFine(bool enabled) {m_fine = enabled;}
    
    Closure<void(int)> onStep;
    NotifyEvent onStepUp;
    NotifyEvent onStepDown;
    
private:
    Gpio *m_pinA;
    Gpio *m_pinB;
    
    bool m_a, m_b;
    bool m_fine = false;
    
    void task();
};
