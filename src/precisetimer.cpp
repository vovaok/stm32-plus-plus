#include "precisetimer.h"

#define PRECISE_TIMER_FREQ  1000000

HardwareTimer *PreciseTimer::s_tim = 0L;

PreciseTimer::PreciseTimer(HardwareTimer::TimerNumber tim)
{
    if (!s_tim)
    {
        s_tim = new HardwareTimer(tim);
        s_tim->setPrescaler(rcc().pClk1() / PRECISE_TIMER_FREQ);
        s_tim->setAutoReloadRegister(0xFFFFFFFF);
        s_tim->generateUpdateEvent();
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
    uint32_t value = s_tim->counter();
    uint32_t dt = value - m_value;
    if (s_tim->tim() != TIM2 && s_tim->tim() != TIM5)
        dt = (uint16_t)dt;
    m_value = value;
    return (float)dt / PRECISE_TIMER_FREQ;
}