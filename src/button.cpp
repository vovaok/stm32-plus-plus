#include "button.h"

Button::Button(PinName pin, bool pullUp, bool inverted) :
    Gpio(pin, pullUp? Gpio::pullUp: Gpio::pullNone),
    mState(false),
    mInverted(inverted),
    mDebounceTime(50),
    mTime(0)
{
    mState = state();
    stmApp()->registerTaskEvent(EVENT(&Button::task));
    stmApp()->registerTickEvent(EVENT(&Button::tick));
}

Button::~Button()
{
    stmApp()->unregisterTaskEvent(EVENT(&Button::task));
    stmApp()->unregisterTickEvent(EVENT(&Button::tick));
}
//---------------------------------------------------------------------------

void Button::task()
{
    if (mTime >= mDebounceTime)
    {
        mTime = 0;
        bool s = state();
        if (s && !mState)
            mPressEvent();
        else if (!s && mState)
            mReleaseEvent();
        mState = s;
    }
}

void Button::tick(int period)
{
    mTime += period;
}
//---------------------------------------------------------------------------

bool Button::state() const
{
    return mInverted ^ read();
}
//---------------------------------------------------------------------------

void Button::setPressEvent(NotifyEvent event)
{
    mPressEvent = event;
}

void Button::setReleaseEvent(NotifyEvent event)
{
    mReleaseEvent = event;
}
//---------------------------------------------------------------------------
