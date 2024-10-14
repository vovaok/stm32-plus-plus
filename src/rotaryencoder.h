#pragma once

#include "gpio.h"
#include "core/coretypes.h"

class RotaryEncoder
{
public:
    RotaryEncoder(Gpio::PinName pinA, Gpio::PinName pinB);
    
    void setPrescaler(int value);
    int prescaler() const {return m_psc;}
    
    int rawValue() const {return m_value;}
    int value() const {return (m_value + m_psc / 2) / m_psc;}
    
    void reset();
    
    Closure<void(int)> onStep;
    NotifyEvent onStepUp;
    NotifyEvent onStepDown;
    
private:
    Gpio *m_pinA;
    Gpio *m_pinB;
    
    bool m_a, m_b;
    int m_psc = 4;
    uint32_t m_value = 0;
    uint32_t m_filter = 0;
    
    void update();
    void task();
};
