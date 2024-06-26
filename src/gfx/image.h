#pragma once

/*
    This is frame buffer intended to draw in memory allocated in the heap
*/

#include "framebuffer.h"
#include "core/bytearray.h"

class Image: public FrameBuffer
{
public:
    Image();    
    Image(int width, int height, PixelFormat pixelFormat=Format_RGB565);
    Image(const char *data, int width, int height, PixelFormat pixelFormat);
    Image(const Image &other);
    Image(Image &&other);
    Image &operator =(const Image &other);
    Image &operator =(Image &&other);

    static Image fromData(const char *data, int size);

    bool isNull() const {return m_buffer.isEmpty();}

//    const uint8_t *bits() const {return reinterpret_cast<const uint8_t *>(m_data.data());}
//    uint8_t *bits() {return reinterpret_cast<uint8_t *>(m_data.data());}

//    const uint16_t *pixels() const {return reinterpret_cast<const uint16_t *>(m_data.data());}
//    uint16_t *pixels() {return reinterpret_cast<uint16_t *>(m_data.data());}

//    const uint16_t *scanLine(int i) const {return reinterpret_cast<const uint16_t *>(m_data.data()) + m_width * i;}
//    uint16_t *scanLine(int i) {return reinterpret_cast<uint16_t *>(m_data.data()) + m_width * i;}

//    virtual void setPixel(int x, int y, uint16_t color);
//    virtual uint16_t pixel(int x, int y);

    void fill(Color color);

    void setPixelColor(int x, int y, Color color);
    Color pixelColor(int x, int y) const;

    bool valid(int x, int y);

private:
    ByteArray m_buffer;
};
