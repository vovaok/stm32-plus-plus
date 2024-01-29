#include "framebuffer.h"
#include "dma2d.h"

FrameBuffer::FrameBuffer(uint8_t *data, int width, int height, PixelFormat pixelFormat) :
    m_data(data),
    m_format(pixelFormat)
{
    m_width = width;
    m_height = height;
    
    switch (pixelFormat)
    {
    case Format_ARGB8888:   m_bpp = 4; break;
    case Format_RGB888:     m_bpp = 3; break;
    case Format_RGB565:     m_bpp = 2; break;
    case Format_ARGB1555:   m_bpp = 2; break;
    case Format_ARGB4444:   m_bpp = 2; break;
    case Format_L8:         m_bpp = 1; break;
    case Format_AL44:       m_bpp = 1; break;
    case Format_AL88:       m_bpp = 2; break;
    }
    
    m_bpl = m_bpp * m_width;
    
//    memset(m_data, 0, m_bpl * m_height);
}

void FrameBuffer::fill(uint32_t color)
{
    if (!m_data)
        return;
#if defined(DMA2D)
    Dma2D dma2d(this);
    dma2d.setSource(color, m_width, m_height);
    dma2d.doTransfer();
#else
    uint8_t *dst = m_data;
    int cnt = m_width * m_height * m_bpp;
    while (cnt--)
        *dst++ = color;
#endif
}

void FrameBuffer::setPixel(int x, int y, uint16_t color)
{
    if ((uint32_t)x < (uint32_t)m_width && (uint32_t)y < (uint32_t)m_height)
    {
        uint8_t *dst = m_data + y*m_bpl + x*m_bpp;
        switch (m_bpp)
        {
        case 1: *dst = color; break;
        case 2: *reinterpret_cast<uint16_t*>(dst) = color; break;
        case 3:
            dst[0] = reinterpret_cast<uint8_t*>(&color)[0];
            dst[1] = reinterpret_cast<uint8_t*>(&color)[1];
            dst[2] = reinterpret_cast<uint8_t*>(&color)[2];
            break;
        case 4: *reinterpret_cast<uint32_t*>(dst) = color; break;
        }
    }
}

uint16_t FrameBuffer::pixel(int x, int y)
{
    union
    {
        uint32_t c32 = 0;
        uint16_t c16;
        uint8_t c8[4];
    };
    if ((uint32_t)x < (uint32_t)m_width && (uint32_t)y < (uint32_t)m_height)
    {
        uint8_t *src = m_data + y*m_bpl + x*m_bpp;
        switch (m_bpp)
        {
        case 1: c8[0] = *src; break;
        case 2: c16 = *reinterpret_cast<uint16_t*>(src); break;
        case 3:
            c8[0] = src[0];
            c8[1] = src[1];
            c8[2] = src[2];
            break;
        case 4: c32 = *reinterpret_cast<uint32_t*>(src); break;
        }
    }
    return c32;
}
    
void FrameBuffer::fillRect(int x, int y, int width, int height, uint16_t color)
{   
#if defined(DMA2D)
    Dma2D dma2d(this, x, y);
    dma2d.setSource(color, width, height);
    dma2d.doTransfer();
#else
    if (width <= 0 || height <= 0)
        return;
    if (x >= m_width || y >= m_height)
        return;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x + width > m_width)
        width = m_width - x;
    if (y + height > m_height)
        height = m_height - y;
        
    int ey = y + height;
    for (int i=y; i<ey; i++)
    {
        uint16_t *dst = scanLine(i) + x;
        int cnt = width;
        while (cnt--)
            *dst++ = color;
    }
#endif
}

void FrameBuffer::copyRect(int x, int y, int width, int height, const uint16_t *buffer)
{
#if defined(DMA2D)
    Dma2D dma2d(this, x, y);
    dma2d.setSource(reinterpret_cast<const uint8_t*>(buffer), width, height);
    dma2d.doTransfer();
#else
    if (width <= 0 || height <= 0)
        return;
    if (x >= m_width || y >= m_height)
        return;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x + width > m_width)
        width = m_width - x;
    if (y + height > m_height)
        height = m_height - y;
        
    int ey = y + height;
    for (int i=y; i<ey; i++)
    {
        uint16_t *dst = scanLine(i) + x;
        int cnt = width;
        while (cnt--)
            *dst++ = *buffer++;
    }
#endif
}