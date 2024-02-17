#include "led.h"

Led::Led(Gpio::PinName pin, bool inverted) :
    Gpio(pin, Gpio::modeOut),
    mCountMig(0),
    mInverted(inverted)
{
    if (mInverted)
        write(1);
}

Led::Led(GPIO_TypeDef *gpio, int pin, bool inverted) :
    Gpio(gpio, pin),
    mCountMig(0),
    mInverted(inverted)
{
    setAsOutput();
    if (mInverted)
        write(1);
}

Led::Led(char portLetter, int pinNumber) :
    Gpio((Gpio::PinName)((((portLetter>='a'?portLetter-'a':portLetter>='A'?portLetter-'A':noPort) & 0x0F) << 4) | (pinNumber & 0x0F)), Gpio::modeOut),
    mCountMig(0),
    mInverted(false)
{
}

Led::~Led()
{
    if (m_blinkTimer)
        delete m_blinkTimer;
}
//---------------------------------------------------------------------------

void Led::setState(bool newState)
{
    setBlinkingEnabled(false);
    write(newState ^ mInverted);
}

void Led::setBlinkInterval(int value)
{
    timer()->setInterval(value);
}

bool Led::isBlinking() const
{
    if (m_blinkTimer)
        return m_blinkTimer->isEnabled();
    return false;
}

void Led::setBlinkingEnabled(bool enabled)
{
    if (enabled)
        timer()->start();
    else if (m_blinkTimer)
        m_blinkTimer->stop();
}

void Led::blink()
{
    on();
    timer()->setSingleShot(false);
    setBlinkingEnabled(true);
}

void Led::blinkOnce()
{
    on();
    timer()->setSingleShot(true);
    setBlinkingEnabled(true);
}

void Led::toggleSkip(int count)
{
    mCountMig++;
    if (mCountMig >= count)
    {
        toggle();
        mCountMig=0;
    }
}

void Led::toggleSkip(int count1, int count2)
{
    mCountMig++;
    if (mCountMig >= count1 + count2)
    {
        on();
        mCountMig=0;
    }
    else if (mCountMig >= count1)
    {
        off();
    }
}

Timer *Led::timer()
{
    if (!m_blinkTimer)
    {
        m_blinkTimer = new Timer;
        m_blinkTimer->setInterval(50);
        m_blinkTimer->onTimeout = EVENT(&Led::toggle);
    }
    return m_blinkTimer;
}
