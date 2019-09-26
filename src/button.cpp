#include "button.h"

Button::Button(PinName pin, bool pullUp, bool inverted) :
    Gpio(pin, pullUp? Gpio::pullUp: Gpio::pullNone),
    mFilter(false),
    mState(false),
    mInverted(inverted),
    mDebounceTime(50),
    mTime(mDebounceTime)
{
    mState = read();
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
        if (mFilter && !mState && mPressEvent)
            mPressEvent();
        else if (!mFilter && mState && mReleaseEvent)
            mReleaseEvent();
        mState = mFilter;
    }
}

void Button::tick(int period)
{
    if (mTime >= period)
        mTime -= period;
    else
        mTime = 0;
}
//---------------------------------------------------------------------------

bool Button::state() const
{
    return mState;
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
