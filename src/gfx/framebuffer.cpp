#include "framebuffer.h"
#include "dma2d.h"

FrameBuffer::FrameBuffer() :
    m_data(nullptr)
{
    m_width = 0;
    m_height = 0;
    m_bpl = 0;
}

FrameBuffer::FrameBuffer(uint8_t *data, int width, int height, PixelFormat pixelFormat) :
    Display(width, height, pixelFormat),
    m_data(data)
{
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
    fillRect(0, 0, m_width, m_height, color);
//    uint32_t *dst = reinterpret_cast<uint32_t*>(m_data);
//    int cnt = m_width * m_height * m_bpp / 4;
//    while (cnt--)
//        *dst++ = color;
#endif
}

void FrameBuffer::drawImage(int x, int y, const Image &img)
{
    if (img.m_pixelFormat == m_pixelFormat && !hasAlphaChannel())
    {
        copyRect(x, y, img.m_width, img.m_height, img.m_data);
        return;
    }

#if defined(DMA2D)
    Dma2D dma2d(this, x, y);
    dma2d.setSource(reinterpret_cast<const uint8_t*>(buffer), width, height, format);
    dma2d.doTransfer();
#else

    int w = img.m_width;
    int h = img.m_height;

    if (w <= 0 || h <= 0)
        return;
    if (x >= m_width || y >= m_height)
        return;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x + w > m_width)
        w = m_width - x;
    if (y + h > m_height)
        h = m_height - y;

    for (int i=0; i<h; i++)
    {
        int yi = y + i;
        for (int j=0; j<w; j++)
        {
            int xj = x + j;
            Color fg = img.pixelColor(j, i);
            Color bg = fromRgb(pixel(xj, yi));
            setPixel(xj, yi, toRgb(Color::blend(fg, bg, 255)));
        }
    }
#endif
}

void FrameBuffer::setPixel(int x, int y, uint32_t color)
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

uint32_t FrameBuffer::pixel(int x, int y) const
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

void FrameBuffer::fillRect(int x, int y, int width, int height, uint32_t color)
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

    if (hasAlphaChannel())
    {
        for (int i=0; i<height; i++)
        {
            int yi = y + i;
            for (int j=0; j<width; j++)
            {
                int xj = x + j;
                Color bg = fromRgb(pixel(xj, yi));
                setPixel(xj, yi, toRgb(Color::blend(color, bg, 255)));
            }
        }
        return;
    }

    //uint8_t *dst = m_data + y * m_bpl + x * m_bpp;
    uint32_t *dst = reinterpret_cast<uint32_t*>(m_data + y * m_bpl + x * m_bpp);

    switch (m_bpp)
    {
    case 0:
        return; // nonsense
    case 1:
        color &= 0xFF;
        color |= (color << 8);
        // no break - this is intended
    case 2:
        color &= 0xFFFF;
        color |= (color << 16);
        break;
    case 3:
        color &= 0xFFFFFF;
    }

    if (m_bpp == 3) // special case for 24-bit pixels
    {
        while (height--)
        {
            int cnt = width;
            while (cnt--)
                *dst++ = color;
            dst += (m_bpl - width) / 4;
        }
    }
    else
    {
        int ww = width * m_bpp / 4;
        uint32_t mask = (1 << (((width * m_bpp) & 3) * 8)) - 1;
        while (height--)
        {
            int cnt = ww;
            while (cnt--)
                *dst++ = color;
            *dst = (*dst & ~mask) | (color & mask); // last pixel(s)
            dst += m_bpl / 4 - ww;
        }
    }
#endif
}

void FrameBuffer::copyRect(int x, int y, int width, int height, const uint8_t *buffer)
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

    uint32_t *dst = reinterpret_cast<uint32_t*>(m_data + y * m_bpl + x * m_bpp);
    const uint32_t *src = reinterpret_cast<const uint32_t*>(buffer);

    while (height--)
    {
        int cnt = (width * m_bpp) / 4;
        while (cnt--)
            *dst++ = *src++;
        dst += (m_bpl - width * m_bpp) / 4;

        //! @todo check last pixel!!!
    }
#endif
}
