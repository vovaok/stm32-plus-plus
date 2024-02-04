#pragma once

#include "display.h"

class FrameBuffer : public Display
{
public:
    FrameBuffer(uint8_t *data, int width, int height, PixelFormat pixelFormat);

    void fill(uint32_t color);
    
    uint8_t *data() {return m_data;}

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