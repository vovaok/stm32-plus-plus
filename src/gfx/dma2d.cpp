#include "dma2d.h"
#include "core/macros.h"

#if defined(DMA2D)

Dma2D::Dma2D(FrameBuffer *fb, int x, int y) :
    m_fb(fb)//,
//    m_maxw(fb->width() - x), m_maxh(fb->height() - y)
{
    //! @todo remember offset instead of ignoring it!
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;

    m_maxw = fb->width() - x;
    m_maxh = fb->height() - y;

    if (m_maxw < 0)
        m_maxw = 0;
    if (m_maxh < 0)
        m_maxh = 0;
    DMA2D->OPFCCR = fb->pixelFormat();
    DMA2D->OMAR = reinterpret_cast<uint32_t>(fb->m_data + y * fb->m_bpl + (x * fb->m_bpp >> 3));
//    DMA2D->OOR = fb->width();
}

void Dma2D::setSize(int width, int height)
{
    m_maxw = width;
    m_maxh = height;
}

void Dma2D::setSource(uint32_t color, int width, int height)
{
    width = MIN(width, m_maxw);
    height = MIN(height, m_maxh);
    DMA2D->OCOLR = color;
    DMA2D->OOR = m_fb->width() - width;
    DMA2D->NLR = (width << 16) | height;
    DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Reg2Mem;
}

void Dma2D::setSource(const uint8_t *data, int width, int height)
{
    int w = MIN(width, m_maxw);
    int h = MIN(height, m_maxh);
    DMA2D->FGMAR = reinterpret_cast<uint32_t>(data);
    DMA2D->FGOR = width - w;
    DMA2D->FGPFCCR = m_fb->pixelFormat() & 0xF;
    DMA2D->OOR = m_fb->width() - w;
    DMA2D->NLR = (w << 16) | h;
    DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2Mem;
}

void Dma2D::setSource(const uint8_t *data, int width, int height, PixelFormat fmt)
{
    int w = MIN(width, m_maxw);
    int h = MIN(height, m_maxh);

    DMA2D->FGMAR = reinterpret_cast<uint32_t>(data);
    DMA2D->FGOR = width - w;
    DMA2D->FGPFCCR = (fmt & 0xF);

    if (fmt == Format_A8 || fmt == Format_A4)
    {
        Color c = m_fb->color();
        uint8_t a = c.alpha();
        uint8_t r = c.r();
        uint8_t g = c.g();
        uint8_t b = c.b();
        if (a != 255)
            DMA2D->FGPFCCR |= (a << 24) | (DMA2D_FGPFCCR_AM_1);
        DMA2D->FGCOLR = (r << 16) | (g << 8) | b;
    }

    DMA2D->BGMAR = DMA2D->OMAR;
    DMA2D->BGOR = m_fb->width() - w;
    DMA2D->BGPFCCR = m_fb->pixelFormat() & 0xF;

    DMA2D->OOR = m_fb->width() - w;
    DMA2D->NLR = (w << 16) | h;
    DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2MemBlend;
}

void Dma2D::setSource(const FrameBuffer *fb, int x, int y)
{
    const uint8_t *data = fb->data() + y * fb->m_bpl + (x * fb->m_bpp >> 3);
    setSource(data, fb->width(), fb->height(), fb->pixelFormat());

    if (fb->opacity() < 255 || fb->hasAlphaChannel())
    {
        uint32_t tmp = DMA2D->FGPFCCR & ~(DMA2D_FGPFCCR_ALPHA_Msk | DMA2D_FGPFCCR_AM_Msk);
        DMA2D->FGPFCCR = tmp | (fb->opacity() << 24) | (DMA2D_FGPFCCR_AM_1);//0);
        DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2MemBlend;
    }
    else if (fb->pixelFormat() != m_fb->pixelFormat())
    {
        DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2MemPfc;
    }
    else
    {
        DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2Mem;
    }
}

void Dma2D::setSource(const FrameBuffer *foreground, const FrameBuffer *background)
{
    int w = MIN(foreground->width(), m_maxw);
    int h = MIN(foreground->height(), m_maxh);
    PixelFormat fg_fmt = foreground->pixelFormat();
    PixelFormat bg_fmt = background->pixelFormat();

    DMA2D->FGMAR = reinterpret_cast<uint32_t>(foreground->data());
    DMA2D->FGOR = foreground->width() - w;
    DMA2D->FGPFCCR = fg_fmt & 0xF;

    if (fg_fmt == Format_A8 || fg_fmt == Format_A4)
    {
        Color c = m_fb->color();
        uint8_t a = c.alpha();
        uint8_t r = c.r();
        uint8_t g = c.g();
        uint8_t b = c.b();
        if (a != 255)
            DMA2D->FGPFCCR |= (a << 24) | (DMA2D_FGPFCCR_AM_1);
        DMA2D->FGCOLR = (r << 16) | (g << 8) | b;
    }

    DMA2D->BGMAR = reinterpret_cast<uint32_t>(background->data());
    DMA2D->BGOR = background->width() - w;
    DMA2D->BGPFCCR = bg_fmt & 0xF;

    if (bg_fmt == Format_A8 || bg_fmt == Format_A4)
    {
        Color c = m_fb->backgroundColor();
        uint8_t a = c.alpha();
        uint8_t r = c.r();
        uint8_t g = c.g();
        uint8_t b = c.b();
        if (a != 255)
            DMA2D->BGPFCCR |= (a << 24) | (DMA2D_BGPFCCR_AM_1);
        DMA2D->BGCOLR = (r << 16) | (g << 8) | b;
    }

    DMA2D->OOR = m_fb->width() - w;
    DMA2D->NLR = (w << 16) | h;
    DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2MemBlend;
}

void Dma2D::start()
{
    if (m_maxw > 0 && m_maxh > 0)
        DMA2D->CR |= DMA2D_CR_START;
}

void Dma2D::wait()
{
    while (DMA2D->CR & DMA2D_CR_START);
}

#endif
