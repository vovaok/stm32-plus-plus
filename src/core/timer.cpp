#include "timer.h"

Timer::Timer() :
    mInterval(0),
    mTime(0L),
    mEnabled(false),
    mUpdated(false),
    mSingleShot(false)
{
    m_taskid = stmApp()->registerTaskEvent(EVENT(&Timer::task));
    m_tickid = stmApp()->registerTickEvent(EVENT(&Timer::tick));
}

Timer::~Timer()
{
    stmApp()->unregisterTaskEvent(m_taskid);
    stmApp()->unregisterTickEvent(m_tickid);
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
            if (mSingleShot)
                mEnabled = false;
        }
    }
}
//---------------------------------------------------------------------------

void Timer::task()
{
    if (mUpdated)
    {
        mUpdated = false;
        if (callback)
            callback();
        if (onTimeout)
            onTimeout();
    }
}
//---------------------------------------------------------------------------

void Timer::start(int interval)
{
    mTime = 0;
    if (interval)
        mInterval = interval;
    mEnabled = true;
}

void Timer::stop()
{
    mEnabled = false;
}
//---------------------------------------------------------------------------
