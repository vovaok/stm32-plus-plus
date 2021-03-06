#ifndef _TIMER_H
#define _TIMER_H

#include <list>
#include "core/core.h"

class Timer
{
private:
    int mInterval;
    unsigned int mTime;
    bool mEnabled;
    bool mUpdated;
    int m_taskid, m_tickid;
    
    NotifyEvent mTimeoutEvent;
    
    void tick(int period);
    void task();
    
public:
    Timer();
    ~Timer();
    
    int interval() const {return mInterval;}
    void setInterval(int interval) {mInterval = interval;}
    
    NotifyEvent timeoutEvent() {return mTimeoutEvent;}
    void setTimeoutEvent(NotifyEvent event) {mTimeoutEvent = event;}
    
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