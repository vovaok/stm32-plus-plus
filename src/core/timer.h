#ifndef _TIMER_H
#define _TIMER_H

#include "core.h"

class Timer
{
private:
    int mInterval;
    unsigned int mTime;
    bool mEnabled;
    bool mUpdated;
    bool mSingleShot;
    int m_taskid, m_tickid;

    void tick(int period);
    void task();

public:
    Timer();
    ~Timer();

    int interval() const {return mInterval;}
    void setInterval(int interval) {mInterval = interval;}

    bool isSingleShot() const {return mSingleShot;}
    void setSingleShot(bool singleShot) {mSingleShot = singleShot;}

    NotifyEvent onTimeout;
    void (*callback)(void) = nullptr;

    NotifyEvent timeoutEvent() {return onTimeout;}
    void setTimeoutEvent(NotifyEvent event) {onTimeout = event;}

//    void reset();
    void start(int interval = 0);
//    void pause();
    void stop();

    int time() const {return mTime;}
    int remainingTime() const {return mInterval - mTime;}
    bool isEnabled() const {return mEnabled;}
    bool isRunning() const {return mEnabled;}
    bool isActive() const {return mEnabled;}
};

#endif
