#include "led.h"

Led::Led(Gpio::PinName pin, bool inverted) :
    Gpio(pin, Gpio::modeOut),
    mCountMig(0),
    mInverted(inverted)
{
}

Led::Led(char portLetter, int pinNumber) :
    Gpio((Gpio::PinName)((((portLetter>='a'?portLetter-'a':portLetter>='A'?portLetter-'A':noPort) & 0x0F) << 4) | (pinNumber & 0x0F)), Gpio::modeOut),
    mCountMig(0),
    mInverted(false)
{
}
//---------------------------------------------------------------------------

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
//---------------------------------------------------------------------------
