#include "touchpanel.h"
#include "core/application.h"

TouchPanel::TouchPanel() :
    m_rawX(0), m_rawY(0), m_rawZ(0),
    m_xOffset(0), m_yOffset(0),
    m_xFactor(32768), m_yFactor(32768)
{
    stmApp()->registerTaskEvent(EVENT(&TouchPanel::task));
}

void TouchPanel::task()
{
    if (!read())
        return;

    int ox = m_rawX;
    int oy = m_rawY;
//    int oz = m_rawZ;

    if (m_pen)
    {
        int press = m_rawZ;
        if (press > m_sensibility)
        {
            m_pressure = press - m_sensibility;

            TouchEvent::Type type = TouchEvent::Move;
            if (m_x < 0)
            {
                type = TouchEvent::Press;
                ox = m_rawX;
                oy = m_rawY;
            }

            int dx = m_rawX - ox;
            int dy = m_rawY - oy;
            if (dx > 1000 || dx < -1000 || dy > 1000 || dy < -1000)
                dx = dy = 0;

            // LPF
            m_rawX = ox + dx / 16;
            m_rawY = oy + dy / 16;

            int tx = m_xOffset + m_rawX * m_xFactor / 32768;
            int ty = m_yOffset + m_rawY * m_yFactor / 32768;

            //! @todo Orientation fix here

            if (m_xySwap)
            {
                m_x = ty;
                m_y = tx;
            }
            else
            {
                m_x = tx;
                m_y = ty;
            }

            touchEvent(type);
        }
        else
        {
            m_pressure = 0;
        }
    }
    else if (m_x >= 0) // was touched
    {
        touchEvent(TouchEvent::Release);
        m_x = m_y = -1;
        m_pressure = 0;
        m_rawX = m_rawY = 0;
    }
}

void TouchPanel::calcCalibration(int width, int height, int left, int top, int right, int bottom)
{
    m_xFactor = 32768 * width / (right - left);
    m_xOffset = -left * m_xFactor / 32768;
    m_yFactor = 32768 * height / (bottom - top);
    m_yOffset = -top * m_yFactor / 32768;
}

void TouchPanel::setCalibration(int xOffset, int yOffset, int xFactor, int yFactor)
{
    m_xOffset = xOffset;
    m_yOffset = yOffset;
    m_xFactor = xFactor;
    m_yFactor = yFactor;
}

void TouchPanel::setSwappedXY(bool swapped)
{
    m_xySwap = swapped;
}

void TouchPanel::setSensibility(int value)
{
    m_sensibility = BOUND(0, value, 4);
}