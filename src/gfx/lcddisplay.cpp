#include "lcddisplay.h"

LcdDisplay::LcdDisplay() :
    Display()
{
    rcc().setPeriphEnabled(LTDC);
    rcc().setPeriphEnabled(DMA2D);
}

void LcdDisplay::setPixel(int x, int y, uint16_t color)
{

}

uint16_t LcdDisplay::pixel(int x, int y)
{

    return 0;
}

void LcdDisplay::fillRect(int x, int y, int width, int height, uint16_t color)
{

}

void LcdDisplay::copyRect(int x, int y, int width, int height, const uint16_t *buffer)
{

}

void LcdDisplay::init()
{
    rcc().configLtdcClock(m_pixelClock);

    //! @todo configure polarity
//    LTDC->GCR &= (uint32_t)GCR_MASK;
//    LTDC->GCR |=  (uint32_t)(LTDC_InitStruct->LTDC_HSPolarity | LTDC_InitStruct->LTDC_VSPolarity | \
//                           LTDC_InitStruct->LTDC_DEPolarity | LTDC_InitStruct->LTDC_PCPolarity);

    #pragma pack(push, 1)
    union
    {
        uint32_t reg;
        struct
        {
            uint16_t v;
            uint16_t h;
        };
    };
    #pragma pack(pop)

    // program HSYNC and VSYNC width
    h = m_HS - 1;
    v = m_VS - 1;
    LTDC->SSCR = reg;

    // program accumulated back porch
    h += m_HBP;
    v += m_VBP;
    LTDC->BPCR = reg;

    // program accumulated active width and height
    h += m_width;
    v += m_height;
    LTDC->AWCR = reg;

    // program total width and height
    h += m_HFP;
    v += m_VFP;
    LTDC->TWCR = reg;

    int bkr = backgroundColor().r();
    int bkg = backgroundColor().g();
    int bkb = backgroundColor().b();
    LTDC->BCCR = (bkr << 16) | (bkg << 8) | bkb;
}