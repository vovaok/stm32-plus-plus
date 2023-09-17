#include "touchscreen.h"

TouchScreen::TouchScreen() :
    m_x(-1), m_y(-1),
    m_pressure(0),
    m_xOffset(0), m_yOffset(0),
    m_xFactor(0), m_yFactor(0)
{
}

void TouchScreen::setCalibration(int xOffset, int yOffset, int xFactor, int yFactor)
{
    m_xOffset = xOffset;
    m_yOffset = yOffset;
    m_xFactor = xFactor;
    m_yFactor = yFactor;
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