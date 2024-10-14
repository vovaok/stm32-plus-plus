#ifndef _ILI9486_H
#define _ILI9486_H

#include "spi.h"
#include "pwmout.h"
#include "gfx/display.h"

#define LCD_WIDTH   480
#define LCD_HEIGHT  320

class ILI9486 : public Display
{
public:
    ILI9486(Spi *spi, Gpio::PinName cs, Gpio::PinName dc, Gpio::PinName rst);
    void setBacklightPin(Gpio::Config pin);
    
    void setBacklight(int percent); // 0 to 255
    
    void init(Orientation ori);
    void setOrientation(Orientation ori);
    
	virtual void fillRect(int x, int y, int width, int height, uint32_t color) override;
    virtual void overlayRect(int x, int y, int width, int height, uint32_t color) override;
    virtual void copyRect(int x, int y, int width, int height, const uint8_t *buffer) override;
    virtual void blendRect(int x, int y, int width, int height, const uint8_t *buffer, PixelFormat format) override;
    virtual void drawBuffer(int x, int y, const FrameBuffer *fb, int sx=0, int sy=0, int sw=-1, int sh=-1) override;

    virtual void setPixel(int x, int y, uint32_t color);
//    virtual uint16_t pixel(int x, int y); // THIS DISPLAY IMPLEMENTATION IS NOT READABLE!!!
    
private:
    Spi *m_spi;
    Gpio *m_cs, *m_dc, *m_rst;
    Gpio::Config m_pwmPin;
    PwmOutput *m_backlightPwm;
    
    void hardwareReset();
    void delay(int ms) {for (int w=ms*20000; --w;);}
    void initReg();
    
    void LCD_WriteReg(uint8_t reg);
    void LCD_WriteData(uint16_t data);
//    void LCD_WriteColor(uint16_t color);
    
    void setArea(int xStart, int yStart, int xEnd, int yEnd);
};

#endif