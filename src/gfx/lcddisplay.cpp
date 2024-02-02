#include "lcddisplay.h"

LcdDisplay::LcdDisplay(int width, int height)
{
    m_width = width;
    m_height = height;
    rcc().setPeriphEnabled(LTDC);
    rcc().setPeriphEnabled(DMA2D);
}

void LcdDisplay::configLayer(int number, FrameBuffer *frameBuffer)
{
    if (number < 1 || number > 2)
        return;

    // choose appropriate register set
    LTDC_Layer_TypeDef *LTDC_Layer = ltdcLayer(number);

    // store framebuffer for later use
    m_layerFB[number - 1] = frameBuffer;

    // program window position
    setLayerPos(number, 0, 0);

    // program pixel format
    LTDC_Layer->PFCR = frameBuffer->pixelFormat();

    // program address of pixel data
    LTDC_Layer->CFBAR = reinterpret_cast<uint32_t>(frameBuffer->m_data);

    // program pitch and line length
    // according to the datasheet: line length = pitch + 3
    int bpl = frameBuffer->m_bpl;
    LTDC_Layer->CFBLR = (bpl << 16) | (bpl + 3);

    // program number of lines
    LTDC_Layer->CFBLNR = frameBuffer->height();

    // enable layer
    LTDC_Layer->CR |= LTDC_LxCR_LEN;
    
    // update registers from shadow ones
    reloadConfig();
}

void LcdDisplay::setLayerPos(int number, int x, int y)
{
    LTDC_Layer_TypeDef *LTDC_Layer = ltdcLayer(number);
    FrameBuffer *fb = m_layerFB[number - 1];
    if (!fb)
        return;

    // program window horizontal position
    int hst = m_timings.HS + m_timings.HBP + x;
    int hsp = hst + fb->width() - 1;
    LTDC_Layer->WHPCR = (hsp << 16) | hst;

    // program window vertical position
    int vst = m_timings.VS + m_timings.VBP + y;
    int vsp = vst + fb->height() - 1;
    LTDC_Layer->WVPCR = (vsp << 16) | vst;

    reloadConfig();
}

void LcdDisplay::setLayerOpacity(int number, uint8_t alpha)
{
    LTDC_Layer_TypeDef *LTDC_Layer = ltdcLayer(number);
    LTDC_Layer->CACR = alpha;
    reloadConfig();
}

void LcdDisplay::setLayerDefaultColor(int number, Color color)
{
    LTDC_Layer_TypeDef *LTDC_Layer = ltdcLayer(number);
    int a = color.alpha();
    int r = color.r();
    int g = color.g();
    int b = color.b();
    LTDC_Layer->DCCR = (a << 24) | (r << 16) | (g << 8) | b;
    reloadConfig();
}

void LcdDisplay::setLayerColorKeying(int number, Color color)
{
    LTDC_Layer_TypeDef *LTDC_Layer = ltdcLayer(number);
    int r = color.r();
    int g = color.g();
    int b = color.b();
    LTDC_Layer->CKCR = (r << 16) | (g << 8) | b;
    LTDC_Layer->CR |= LTDC_LxCR_COLKEN;
    reloadConfig();
}

void LcdDisplay::setEnabled(bool enabled)
{
    if (enabled)
        LTDC->GCR |= LTDC_GCR_LTDCEN;
    else
        LTDC->GCR &= ~LTDC_GCR_LTDCEN;
}

void LcdDisplay::setPixel(int x, int y, uint16_t color)
{
    if (m_layerFB[1])
        m_layerFB[1]->setPixel(x, y, color);
    else if (m_layerFB[0])
        m_layerFB[0]->setPixel(x, y, color);
}

uint16_t LcdDisplay::pixel(int x, int y)
{
    if (m_layerFB[1])
        return m_layerFB[1]->pixel(x, y);
    else if (m_layerFB[0])
        return m_layerFB[0]->pixel(x, y);
    return 0;
}

void LcdDisplay::fillRect(int x, int y, int width, int height, uint16_t color)
{
    if (m_layerFB[1])
        m_layerFB[1]->fillRect(x, y, width, height, color);
    else if (m_layerFB[0])
        m_layerFB[0]->fillRect(x, y, width, height, color);
}

void LcdDisplay::copyRect(int x, int y, int width, int height, const uint16_t *buffer)
{
    if (m_layerFB[1])
        m_layerFB[1]->copyRect(x, y, width, height, buffer);
    else if (m_layerFB[0])
        m_layerFB[0]->copyRect(x, y, width, height, buffer);
}

void LcdDisplay::init(const Timings &timings)
{
    m_timings = timings;
    m_timings.pixelClock = rcc().configLtdcClock(m_timings.pixelClock);

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
    h = m_timings.HS - 1;
    v = m_timings.VS - 1;
    LTDC->SSCR = reg;

    // program accumulated back porch
    h += m_timings.HBP;
    v += m_timings.VBP;
    LTDC->BPCR = reg;

    // program accumulated active width and height
    h += m_width;
    v += m_height;
    LTDC->AWCR = reg;

    // program total width and height
    h += m_timings.HFP;
    v += m_timings.VFP;
    LTDC->TWCR = reg;

    int bkr = backgroundColor().r();
    int bkg = backgroundColor().g();
    int bkb = backgroundColor().b();
    LTDC->BCCR = (bkr << 16) | (bkg << 8) | bkb;
}

void LcdDisplay::setFps(int fps)
{
    int tw = m_timings.HS + m_timings.HBP + m_width + m_timings.HFP;
    int th = m_timings.VS + m_timings.VBP + m_height + m_timings.VFP;
    m_timings.pixelClock = tw * th * fps;
    m_timings.pixelClock = rcc().configLtdcClock(m_timings.pixelClock);
}

int LcdDisplay::getFps()
{
    int tw = m_timings.HS + m_timings.HBP + m_width + m_timings.HFP;
    int th = m_timings.VS + m_timings.VBP + m_height + m_timings.VFP;
    int twh = tw * th;
    return (m_timings.pixelClock + twh/2) / twh;
}

void LcdDisplay::reloadConfig(bool immediately)
{
    if (immediately)
        LTDC->SRCR = LTDC_SRCR_IMR;
    else // reload shadow registers during the vertical blanking period
        LTDC->SRCR = LTDC_SRCR_VBR;
}

LTDC_Layer_TypeDef *LcdDisplay::ltdcLayer(int number)
{
    switch (number)
    {
    case 1: return LTDC_Layer1;
    case 2: return LTDC_Layer2;
    default: return nullptr;
    }

}