#include "touchscreen.h"

TouchScreen::TouchScreen() :
    m_x(-1), m_y(-1),
    m_pressure(0)
{
}

void TouchScreen::touchEvent(TouchEvent::Type type)
{
    TouchEvent e;
    e.m_x = m_x;
    e.m_y = m_y;
    e.m_pressure = m_pressure;
    e.m_type = type;
    if (onTouch)
        onTouch(&e);
}