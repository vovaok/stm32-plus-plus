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
    DMA2D->OMAR = reinterpret_cast<uint32_t>(fb->m_data + y * fb->m_bpl + x * fb->m_bpp);
//    DMA2D->OOR = fb->width();
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
    
//    DMA2D->OMAR += m_fb->m_bpl * 100;
    
    DMA2D->OOR = m_fb->width() - w;
    DMA2D->NLR = (w << 16) | h;
    DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2MemBlend;
}

//void Dma2D::setSource(const Image &img, uint8_t opacity)
//{
//    int w = MIN(img.width(), m_maxw);
//    int h = MIN(img.height(), m_maxh);
//    DMA2D->FGMAR = reinterpret_cast<uint32_t>(img.data());
//    DMA2D->FGOR = img.width() - w;
//    DMA2D->OOR = m_fb->width() - w;
////    DMA2D->FGPFCCR = (opacity << 24) | (img.pixelFormat() & 0xF);
////    if (m_fb->pixelFormat() == img.pixelFormat() && opacity == 255)
//    {
//        DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2Mem;
//    }
////    else
////    {
////        if (opacity < 255)
////            DMA2D->FGPFCCR |= (2 << DMA2D_FGPFCCR_AM_Pos);
////        DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2MemPfc;
////    }
//    //! @todo dodelat!
//}

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
