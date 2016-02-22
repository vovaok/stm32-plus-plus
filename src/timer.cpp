#include "timer.h"

Timer::Timer() :
    mInterval(0),
    mTime(0L),
    mEnabled(false),
    mUpdated(false)
{
    stmApp()->registerTaskEvent(EVENT(&Timer::task));
    stmApp()->registerTickEvent(EVENT(&Timer::tick));   
}

Timer::~Timer()
{
    stmApp()->unregisterTaskEvent(EVENT(&Timer::task));
    stmApp()->unregisterTickEvent(EVENT(&Timer::tick));
}
//---------------------------------------------------------------------------

void Timer::tick(int period)
{
    if (!mEnabled)
        return;
    
    mTime += period;
    if (mInterval)
    {
        if (mTime >= mInterval)
        {
            mTime -= mInterval;
            mUpdated = true;
        }
    }
}
//---------------------------------------------------------------------------

void Timer::task()
{
    if (mUpdated)
    {
        mUpdated = false;
        if (mTimeoutEvent)
            mTimeoutEvent();
    }
}
//---------------------------------------------------------------------------

//void Timer::reset()
//{
//    mTime = 0;
//}

void Timer::start(int interval)
{
    mTime = 0;
    if (interval)
        mInterval = interval;
    mEnabled = true;
}

//void Timer::pause()
//{
//    mEnabled = false; 
//}

void Timer::stop()
{
    mEnabled = false;
//    mTime = 0;
}
//---------------------------------------------------------------------------
