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

void FrameBuffer::setPixel(int x, int y, uint32_t color)
{
    if ((uint32_t)x < (uint32_t)m_width && (uint32_t)y < (uint32_t)m_height)
    {
        uint8_t *dst = m_data + y*m_bpl + (x*m_bpp >> 3);
        switch (m_bpp)
        {
        case 4:
//            if (x & 1)
//                *dst = (*dst & 0x0F) | (color << 4);
//            else
//                *dst = (*dst & 0xF0) | (color & 0x0F);
            // MSB aligned
            if (x & 1)
                *dst = (*dst & 0x0F) | (color & 0xF0);
            else
                *dst = (*dst & 0xF0) | (color >> 4);
            break;
        case 8: *dst = color; break;
        case 16: *reinterpret_cast<uint16_t*>(dst) = color; break;
        case 24:
            dst[0] = reinterpret_cast<uint8_t*>(&color)[0];
            dst[1] = reinterpret_cast<uint8_t*>(&color)[1];
            dst[2] = reinterpret_cast<uint8_t*>(&color)[2];
            break;
        case 32: *reinterpret_cast<uint32_t*>(dst) = color; break;
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
        uint8_t *src = m_data + y*m_bpl + (x*m_bpp >> 3);
        switch (m_bpp)
        {
        case 4: c32 = x & 1? *src & 0xF0: *src << 4; break; // MSB-aligned
        case 8: c8[0] = *src; break;
        case 16: c16 = *reinterpret_cast<uint16_t*>(src); break;
        case 24:
            c8[0] = src[0];
            c8[1] = src[1];
            c8[2] = src[2];
            break;
        case 32: c32 = *reinterpret_cast<uint32_t*>(src); break;
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

    uint32_t *dst = reinterpret_cast<uint32_t*>(m_data + y * m_bpl + (x * m_bpp >> 3));

    switch (m_bpp)
    {
    case 0:
        return; // nonsense
    case 8:
        color &= 0xFF;
        color |= (color << 8);
        // no break - this is intended
    case 16:
        color &= 0xFFFF;
        color |= (color << 16);
        break;
    case 24:
        color &= 0xFFFFFF;
    }

    if (m_bpp == 24) // special case for 24-bit pixels
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
        int ww = width * m_bpp / 32;
        uint32_t mask = (1 << (((width * m_bpp >> 3) & 3) * 8)) - 1;
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

void FrameBuffer::overlayRect(int x, int y, int width, int height, uint32_t color)
{
#if 0 && defined(DMA2D)
    Dma2D dma2d(this, x, y);
    dma2d.setSource(color, width, height, !!blenderEnabled!!);
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

    for (int i=0; i<height; i++)
    {
        int yi = y + i;
        for (int j=0; j<width; j++)
        {
            int xj = x + j;
            Color bg = fromRgb(pixel(xj, yi));
            Color fg = fromRgb(color);
            setPixel(xj, yi, toRgb(Color::blend(fg, bg, 255)));
        }
    }
#endif
}

void FrameBuffer::copyRect(int x, int y, int width, int height, const uint8_t *buffer)
{
#if defined(DMA2D)
    Dma2D dma2d(this, x, y);
    dma2d.setSource(buffer, width, height);
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

    uint32_t *dst = reinterpret_cast<uint32_t*>(m_data + y * m_bpl + (x * m_bpp >> 3));
    const uint32_t *src = reinterpret_cast<const uint32_t*>(buffer);

    uint32_t mask = (1 << (((width * m_bpp >> 3) & 3) * 8)) - 1;

    while (height--)
    {
        int cnt = (width * m_bpp >> 3) / 4;
        while (cnt--)
            *dst++ = *src++;
        // last pixel:
        if (mask)
            *dst++ = (*dst & ~mask) | (*src++ & mask); // last pixel(s)
        dst += (m_bpl - (width * m_bpp >> 3)) / 4;

        //! @todo check last pixel!!!
    }
#endif
}

void FrameBuffer::blendRect(int x, int y, int width, int height, const uint8_t *buffer, PixelFormat format)
{
#if defined(DMA2D)
    Dma2D dma2d(this, x, y);
    dma2d.setSource(buffer, width, height, format);
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

    FrameBuffer src(const_cast<uint8_t *>(buffer), width, height, format);

    Color fg;

    for (int i=0; i<height; i++)
    {
        int yi = y + i;
        for (int j=0; j<width; j++)
        {
            int xj = x + j;
            if (format == Format_A8 || format == Format_A4)
            {
                fg = color();
                fg.setAlpha(src.pixel(j, i) * m_opacity / 255);
            }
            else
            {
                fg = src.fromRgb(src.pixel(j, i));
            }
            Color bg = fromRgb(pixel(xj, yi));
            setPixel(xj, yi, toRgb(Color::blend(fg, bg, 255)));
        }
    }

#endif
}

void FrameBuffer::drawBuffer(int x, int y, const FrameBuffer *fb, int sx, int sy, int sw, int sh)
{
    if (x >= m_width || y >= m_height)
        return; // nothing to do
    
    if (x < 0)
    {
        sx -= x;
        x = 0;
    }
    if (y < 0)
    {
        sy -= y;
        y = 0;
    }
    
    if (sw <= 0)
        sw = fb->m_width;
    if (sh <= 0)
        sh = fb->m_height;
    
    if (x + sw > m_width)
        sw = m_width - x;
    if (y + sh > m_height)
        sh = m_height - y;
    
    if (sx >= fb->m_width || sy >= fb->m_height)
        return; // nothing to do
    
#if defined(DMA2D)
    Dma2D dma2d(this, x, y);
    dma2d.setSize(sw, sh);
    dma2d.setSource(fb, sx, sy);
    dma2d.doTransfer();
#else
    Color fg;
    PixelFormat format = fb->pixelFormat();

    for (int i=0; i<sh; i++)
    {
        int yi = y + i;
        int ys = sy + i;
        for (int j=0; j<sw; j++)
        {
            int xj = x + j;
            int xs = sx + j;
            if (format == Format_A8 || format == Format_A4)
            {
                fg = color();
                fg.setAlpha(fb->pixel(xs, ys) * fb->opacity() / 255);
            }
            else
            {
                fg = fb->fromRgb(fb->pixel(xs, ys));
                fg.setAlpha(fg.alpha() * fb->opacity() / 255);
            }
            Color bg = fromRgb(pixel(xj, yi));
            setPixel(xj, yi, toRgb(Color::blend(fg, bg, 255)));
        }
    }
#endif
}
