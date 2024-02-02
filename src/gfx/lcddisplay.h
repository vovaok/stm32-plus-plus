#pragma once

#include "rcc.h"
#include "display.h"
#include "framebuffer.h"

#if defined(LTDC)

class LcdDisplay : public Display
{
public:
    LcdDisplay(int width, int height);

    struct Timings
    {
        int pixelClock; //!< Pixel clock, Hz
        int HS;  //!< Horizontal SYNC width
        int HBP; //!< Horizontal back porch
        int HFP; //!< Horizontal front porch
        int VS;  //!< Vertical SYNC width
        int VBP; //!< Vertical back porch
        int VFP; //!< Vertical front porch
    };

    void init(const Timings &timings);
    void setFps(int fps);
    int getFps();

    void configLayer(int number, FrameBuffer *frameBuffer);
    void setLayerPos(int number, int x, int y);
    void setLayerOpacity(int number, uint8_t alpha);
    void setLayerDefaultColor(int number, Color color);
    void setLayerColorKeying(int number, Color color);

    void setEnabled(bool enabled);

    virtual void setPixel(int x, int y, uint16_t color) override;
    virtual uint16_t pixel(int x, int y) override;

protected:
    virtual void fillRect(int x, int y, int width, int height, uint16_t color) override;
    virtual void copyRect(int x, int y, int width, int height, const uint16_t *buffer) override;

    // LCD panel timings (should be filled in child class):
    Timings m_timings {0};

private:
    FrameBuffer *m_layerFB[2] = {nullptr, nullptr};
    void reloadConfig(bool immediately = true);
    // choose appropriate register set
    static LTDC_Layer_TypeDef *ltdcLayer(int number);
};

#endif
