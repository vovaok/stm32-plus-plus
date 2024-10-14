#include "rotaryencoder.h"
#include "core/application.h"

RotaryEncoder::RotaryEncoder(Gpio::PinName pinA, Gpio::PinName pinB)
{
    if (pinA == Gpio::noPin || pinB == Gpio::noPin)
        return; // discard config
    
    m_pinA = new Gpio(pinA, Gpio::pullUp);
    m_pinB = new Gpio(pinB, Gpio::pullUp);
    m_a = m_pinA->read();
    m_b = m_pinB->read();
    
    stmApp()->registerTaskEvent(EVENT(&RotaryEncoder::task));
    
    m_pinA->configInterrupt(EVENT(&RotaryEncoder::update), Gpio::BothEdges);
}

void RotaryEncoder::setPrescaler(int value)
{
    if (value < 1)
        m_psc = 1;
    else
        m_psc = value;
}

void RotaryEncoder::reset()
{
    m_value = 0;
    m_filter = 0;
}

void RotaryEncoder::update()
{
    bool a = m_pinA->read();
    bool b = m_pinB->read();
    
    if (a != m_a)
    {
        if (a == b)
            --m_value;
        else
            m_value++;
    }
    if (b != m_b)
    {
        if (a == b)
            m_value++;
        else
            --m_value;
    }
    
    m_a = a;
    m_b = b;
}

void RotaryEncoder::task()
{
    update();
    
    int delta = m_value - m_filter;
    
    if (delta > m_psc / 2)
    {
        m_filter += m_psc;
        if (onStepUp)
            onStepUp();
        if (onStep)
            onStep(1);
    }
    else if (delta < -m_psc / 2)
    {
        m_filter -= m_psc;
        if (onStepDown)
            onStepDown();
        if (onStep)
            onStep(-1);
    }
}
