#pragma once

#include "display.h"

class FrameBuffer : public Display
{
public:
    enum PixelFormat
    {
        Format_ARGB8888 = 0,
        Format_RGB888   = 1,
        Format_RGB565   = 2,
        Format_ARGB1555 = 3,
        Format_ARGB4444 = 4,
        Format_L8       = 5, // 8-bit luminance
        Format_AL44     = 6, // 4-bit alpha, 4-bit luminance
        Format_AL88     = 7, // 8-bit alpha, 8-bit luminance
    };
    
    FrameBuffer(uint8_t *data, int width, int height, PixelFormat pixelFormat);
    
    void fill(uint32_t color);
    
    virtual void setPixel(int x, int y, uint16_t color) override;
    virtual uint16_t pixel(int x, int y) override;
    
    PixelFormat pixelFormat() const {return m_format;}
    
protected:
    void fillRect(int x, int y, int width, int height, uint16_t color);
    void copyRect(int x, int y, int width, int height, const uint16_t *buffer);    
    
private:
    uint8_t *m_data;
    PixelFormat m_format;
    int m_bpp; // bytes per pixel
    int m_bpl; // bytes per line

    friend class LcdDisplay;
    friend class Dma2D;
    
    uint16_t *scanLine(int y) {return reinterpret_cast<uint16_t*>(m_data + y*m_bpl);}
};