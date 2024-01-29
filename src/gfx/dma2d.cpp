#include "dma2d.h"

Dma2D::Dma2D(FrameBuffer *fb, int x, int y) :
    m_fb(fb)
{
    DMA2D->OPFCCR = fb->pixelFormat();
    DMA2D->OMAR = reinterpret_cast<uint32_t>(fb->m_data + y * fb->m_bpl + x * fb->m_bpp);
//    DMA2D->OOR = fb->width();
}

void Dma2D::setSource(uint32_t color, int width, int height)
{
    DMA2D->OCOLR = color;
    DMA2D->OOR = m_fb->width() - width;
    DMA2D->NLR = (width << 16) | height;
    DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Reg2Mem;
}

void Dma2D::setSource(const uint8_t *data, int width, int height)
{
    DMA2D->FGMAR = reinterpret_cast<uint32_t>(data);
    DMA2D->FGOR = 0;
    DMA2D->FGPFCCR = m_fb->pixelFormat() & 0xF;
    DMA2D->OOR = m_fb->width() - width;
    DMA2D->NLR = (width << 16) | height;
    DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2Mem;
}

void Dma2D::setSource(const Image &img, uint8_t opacity)
{
    DMA2D->FGMAR = reinterpret_cast<uint32_t>(img.bits());
    DMA2D->FGOR = 0;//img.width();
    DMA2D->OOR = m_fb->width() - img.width();
//    DMA2D->FGPFCCR = (opacity << 24) | (img.pixelFormat() & 0xF);
//    if (m_fb->pixelFormat() == img.pixelFormat() && opacity == 255)
    {
        DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2Mem;
    }
//    else
//    {
//        if (opacity < 255)
//            DMA2D->FGPFCCR |= (2 << DMA2D_FGPFCCR_AM_Pos);
//        DMA2D->CR = (DMA2D->CR & ~DMA2D_CR_MODE_Msk) | Mode_Mem2MemPfc;
//    }    
}

void Dma2D::start()
{
    DMA2D->CR |= DMA2D_CR_START;
}

void Dma2D::wait()
{
    while (DMA2D->CR & DMA2D_CR_START);
}