#ifndef _PRECISETIMER_H
#define _PRECISETIMER_H

#ifndef PRECISE_TIMER
#define PRECISE_TIMER   Tim12
#endif

#include "hardwaretimer.h"

class PreciseTimer
{
public:
    PreciseTimer(HardwareTimer::TimerNumber tim = HardwareTimer::PRECISE_TIMER);

    void reset();
    float delta();
//    bool isValid() const {return m_valid;}

private:
    static HardwareTimer *s_tim;
    uint32_t m_value;
//    bool m_valid;
};

#endif