#pragma once

#include "stm32.h"
#include "framebuffer.h"
#include "image.h"

#if defined(DMA2D)

class Dma2D
{
public:
    Dma2D(FrameBuffer *frameBuffer, int x=0, int y=0);
    void setSource(uint32_t color, int width, int height);
    void setSource(const uint8_t *data, int width, int height);
    void setSource(const uint8_t *data, int width, int height, PixelFormat fmt);
//    void setSource(const Image &img, uint8_t opacity=255);

    void start();
    void wait();

    void doTransfer() {start(); wait();}

private:
    FrameBuffer *m_fb;
    int m_maxw, m_maxh;

    enum Mode
    {
        Mode_Mem2Mem = 0 << DMA2D_CR_MODE_Pos,
        Mode_Mem2MemPfc = 1 << DMA2D_CR_MODE_Pos,
        Mode_Mem2MemBlend = 2 << DMA2D_CR_MODE_Pos,
        Mode_Reg2Mem = 3 << DMA2D_CR_MODE_Pos
    };
};

#endif