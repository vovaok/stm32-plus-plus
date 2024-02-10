#pragma once

#include "display.h"

class Image;

class FrameBuffer : public Display
{
public:
    FrameBuffer(); // construct empty/invalid frame buffer
    FrameBuffer(uint8_t *data, int width, int height, PixelFormat pixelFormat);

    void fill(uint32_t color);
//    virtual void drawImage(int x, int y, const Image &img) override;
    
    uint8_t *data() {return m_data;}
    const uint8_t *data() const {return m_data;}

    virtual void setPixel(int x, int y, uint32_t color) override;
    virtual uint32_t pixel(int x, int y) const override;
    virtual bool isReadable() const override {return true;}

protected:
    uint8_t *m_data;

    virtual void fillRect(int x, int y, int width, int height, uint32_t color) override;
    virtual void overlayRect(int x, int y, int width, int height, uint32_t color) override;
    virtual void copyRect(int x, int y, int width, int height, const uint8_t *buffer) override;
    virtual void blendRect(int x, int y, int width, int height, const uint8_t *buffer, PixelFormat format) override;

private:
    friend class LcdDisplay;
    friend class Dma2D;

    // make these private:
    using Display::m_width;
    using Display::m_height;

    uint8_t *scanLine(int y) {return reinterpret_cast<uint8_t*>(m_data + y*m_bpl);}
};
