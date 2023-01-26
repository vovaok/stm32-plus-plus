#include "button.h"

Button::Button(PinName pin, bool pullUp, bool inverted) :
    Gpio(pin, pullUp? Gpio::pullUp: Gpio::pullNone),
    mFilter(false),
    mState(false),
    mInverted(inverted),
    mDebounceTime(50),
    mHoldTime(0),
    mTime(mDebounceTime)
{
    mState = mInverted ^ read();
    mFilter = mState;
    m_taskid = stmApp()->registerTaskEvent(EVENT(&Button::task));
    m_tickid = stmApp()->registerTickEvent(EVENT(&Button::tick));
}

Button::~Button()
{
    stmApp()->unregisterTaskEvent(m_taskid);
    stmApp()->unregisterTickEvent(m_tickid);
}
//---------------------------------------------------------------------------

void Button::task()
{
    bool s = mInverted ^ read();
    if (s != mFilter)
        mTime = mDebounceTime;
    mFilter = s;
        
    if (!mTime)
    {
        if (mFilter && !mState)
        {
            mHoldTime = 0;
            if (onPress)
                onPress();
        }
        else if (!mFilter && mState)
        {
            if (onRelease)
                onRelease();
            if (onClick && mHoldTime < 300)
                onClick();
        }
        mState = mFilter;
    }
}

void Button::tick(int period)
{
    if (mTime >= period)
        mTime -= period;
    else
        mTime = 0;
    
    if (mState)
        mHoldTime += period;
    else
        mHoldTime = 0;
}
//---------------------------------------------------------------------------

bool Button::state() const
{
    return mState;
}
    
int Button::holdTime() const 
{
    return mHoldTime;
}
    
bool Button::isHolding() const
{
    return mState && mHoldTime >= 300;
}
//---------------------------------------------------------------------------
