#include "rotaryencoder.h"

RotaryEncoder::RotaryEncoder(Gpio::PinName pinA, Gpio::PinName pinB)
{
    m_pinA = new Gpio(pinA);
    m_pinB = new Gpio(pinB);
    m_a = m_pinA->read();
    m_b = m_pinB->read();
}

void RotaryEncoder::task()
{
    bool a = m_pinA->read();
    bool b = m_pinB->read();
    
    int step = 0;
    
    if (a != m_a)
    {
        if (a == b)
            step = -1;
        else
            step = 1;
    }
    if (b != m_b)
    {
        if (a == b)
            step = 1;
        else
            step = -1;
    }
    
    m_a = a;
    m_b = b;
    
    if (step)
    {
        if (step > 0)
        {
            if (onStepUp)
                onStepUp();
        }
        else
        {
            if (onStepDown)
                onStepDown();
        }
        
        if (onStep)
            onStep(step);
    }
}
