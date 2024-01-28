#pragma once

#include "rcc.h"
#include "display.h"

class LcdDisplay : public Display
{
public:
    LcdDisplay();

    virtual void setPixel(int x, int y, uint16_t color) override;
    virtual uint16_t pixel(int x, int y) override;

protected:
    virtual void fillRect(int x, int y, int width, int height, uint16_t color) override;
    virtual void copyRect(int x, int y, int width, int height, const uint16_t *buffer) override;

    void init();

    // LCD panel timings (should be filled in child class):
    int m_pixelClock = 0;
    int m_HS = 0, m_VS = 0;   // sync width
    int m_HBP = 0, m_VBP = 0; // back porch
    int m_HFP = 0, m_VFP = 0; // front porch
};
