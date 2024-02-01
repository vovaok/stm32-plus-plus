#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

#include "core/coretypes.h"

#pragma pack(push,1)
class TouchEvent
{
public:
    enum Type : uint8_t
    {
        Press,
        Release,
        Move
    };

    TouchEvent() : m_x(0), m_y(0), m_pressure(0) {}
    int x() const {return m_x;}
    int y() const {return m_y;}
    int pressure() const {return m_pressure;}
    Type type() const {return m_type;}

private:
    int16_t m_x;
    int16_t m_y;
    uint8_t m_pressure;
    Type m_type;
    friend class TouchScreen;
};
#pragma pack(pop)

//! Base class for touch-screen devices
class TouchScreen
{
public:
    TouchScreen();

    bool isTouched() const {return m_pressure > 0;}
    int x() const {return m_x;}
    int y() const {return m_y;}
    int pressure() const {return m_pressure;}

    Closure<void(TouchEvent *)> onTouch;

protected:
    int m_x, m_y;
    int m_pressure;

    void touchEvent(TouchEvent::Type type);
};

#endif
