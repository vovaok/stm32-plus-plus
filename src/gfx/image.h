#pragma once

/*
    This is 16-bit depth image buffer intended to draw in memory
*/

#include "display.h"
#include "core/bytearray.h"

//! @todo Maybe inherit Image from FrameBuffer?

class Image: public Display
{
public:
    Image();
    Image(int width, int height);
    Image(const char *data, int width, int height);

    static Image fromData(const char *data, int size);

    bool isNull() const {return m_data.isEmpty();}

    const uint8_t *bits() const {return reinterpret_cast<const uint8_t *>(m_data.data());}
    uint8_t *bits() {return reinterpret_cast<uint8_t *>(m_data.data());}

    const uint16_t *pixels() const {return reinterpret_cast<const uint16_t *>(m_data.data());}
    uint16_t *pixels() {return reinterpret_cast<uint16_t *>(m_data.data());}

    const uint16_t *scanLine(int i) const {return reinterpret_cast<const uint16_t *>(m_data.data()) + m_width * i;}
    uint16_t *scanLine(int i) {return reinterpret_cast<uint16_t *>(m_data.data()) + m_width * i;}

    virtual void setPixel(int x, int y, uint16_t color);
    virtual uint16_t pixel(int x, int y);

    void setPixelColor(int x, int y, Color color);
    Color pixelColor(int x, int y);

    bool valid(int x, int y);

    void fill(uint16_t color);

protected:
    void fillRect(int x, int y, int width, int height, uint16_t color);
    void copyRect(int x, int y, int width, int height, const uint16_t *buffer);

private:
    ByteArray m_data;
};
