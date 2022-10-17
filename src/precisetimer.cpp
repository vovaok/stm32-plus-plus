#include "precisetimer.h"

#define PRECISE_TIMER_FREQ  1000000

HardwareTimer *PreciseTimer::s_tim = 0L;

PreciseTimer::PreciseTimer()
{
    if (!s_tim)
    {
        s_tim = new HardwareTimer(HardwareTimer::PRECISE_TIMER);
        s_tim->setPrescaler(Rcc::pClk1() / PRECISE_TIMER_FREQ);
        s_tim->setAutoReloadRegister(0xFFFF);
        s_tim->start();
    }
    reset();
}

void PreciseTimer::reset()
{
    m_value = s_tim->counter();
}

float PreciseTimer::delta()
{
    uint16_t value = s_tim->counter();
    uint16_t dt = value - m_value;
    m_value = value;
    return (float)dt / PRECISE_TIMER_FREQ;
}