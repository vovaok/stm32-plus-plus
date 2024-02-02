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

// base class for touch-screen devices
class TouchScreen
{
public:
    TouchScreen();
    void setCalibration(int xOffset, int yOffset, int xFactor, int yFactor);
    void calcCalibration(int width, int height, int left, int top, int right, int bottom);
    void setSwappedXY(bool swapped);
//    void setOrientation(ScreenOrientation ori) {m_orientation = ori;}
    void setSensibility(int value); // 0 .. 4

    bool isTouched() const {return m_pressure > 0;}
    int x() const {return m_x;}
    int y() const {return m_y;}
    int pressure() const {return m_pressure;}

    Closure<void(TouchEvent *)> onTouch;

protected:
    int m_rawX = 0, m_rawY = 0, m_rawZ = 0;
    bool m_pen;
    virtual bool read() = 0;

private:
    int m_x, m_y;
    int m_pressure;
    int m_xOffset, m_yOffset;
    int m_xFactor, m_yFactor;
    int m_sensibility = 2;
    bool m_xySwap = false;
//    ScreenOrientation m_orientation;
    void task();
    void touchEvent(TouchEvent::Type type);
};

#endif
