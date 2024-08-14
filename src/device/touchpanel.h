#pragma once

#include "gfx/touchscreen.h"

//! Abstract base class for hardware touch panels
class TouchPanel : public TouchScreen
{
public:
    TouchPanel();

    void setCalibration(int xOffset, int yOffset, int xFactor, int yFactor);
    void calcCalibration(int width, int height, int left, int top, int right, int bottom);
    void setSwappedXY(bool swapped);
//    void setOrientation(ScreenOrientation ori) {m_orientation = ori;}
    void setSensibility(int value); // 0 .. 4

protected:
    int m_rawX, m_rawY, m_rawZ;
    int m_sensibility = 2;
    bool m_pen;
    virtual bool read() = 0;

private:
    int m_xOffset, m_yOffset;
    int m_xFactor, m_yFactor;
    bool m_xySwap = false;
//    ScreenOrientation m_orientation;

    void task();
};