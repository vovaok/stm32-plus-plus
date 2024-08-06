#ifndef _ST7796_H
#define _ST7796_H

#include "spi.h"
#include "pwmout.h"
#include "gfx/display.h"

#define LCD_WIDTH   480
#define LCD_HEIGHT  320

class ST7796 : public Display
{
public:
    ST7796(Spi *spi, Gpio::PinName cs, Gpio::PinName dc, Gpio::PinName rst);
    void setBacklightPin(Gpio::Config pin);
    
    void setBacklight(int percent); // 0 to 255
    
    void init(Orientation ori);
    void setOrientation(Orientation ori);
    
	virtual void fillRect(int x, int y, int width, int height, uint32_t color) override;
    virtual void overlayRect(int x, int y, int width, int height, uint32_t color) override;
    virtual void copyRect(int x, int y, int width, int height, const uint8_t *buffer) override;
    virtual void blendRect(int x, int y, int width, int height, const uint8_t *buffer, PixelFormat format) override;
    virtual void drawBuffer(int x, int y, const FrameBuffer *fb, int sx=0, int sy=0, int sw=-1, int sh=-1) override;

    virtual void setPixel(int x, int y, uint32_t color) override;
    virtual uint32_t pixel(int x, int y) const override;
    virtual bool isReadable() const override final {return true;}
    
    void readRect(int x, int y, int w, int h, uint8_t *buffer);
    
private:
    Spi *m_spi;
    Gpio *m_cs, *m_dc, *m_rst;
    Gpio::Config m_pwmPin;
    PwmOutput *m_backlightPwm;
    uint16_t m_fillColor; // must not be in the stack!
    
    const int m_spiBaudrate = 10000000;
    
    void hardwareReset();
    void delay(int ms) {for (int w=ms*20000; --w;);}
    void initReg();
    
    enum Instruction : uint8_t
    {
        NOP         = 0x00,
        SWRESET     = 0x01,
        RDDID       = 0x04,
        RDDST       = 0x09, // 
        RDMODE      = 0x0A, // 
        RDMADCTL    = 0x0B, // 
        RDPIXFMT    = 0x0C, // 
        RDIMGFMT    = 0x0D, // 
        RDSELFDIAG  = 0x0F, // 

        SLPIN       = 0x10, // 
        SLPOUT      = 0x11, // 
        PTLON       = 0x12, // 
        NORON       = 0x13, // 

        INVOFF      = 0x20, // 
        INVON       = 0x21, // 
        //GAMMASET    = 0x26, //
        DISPOFF     = 0x28, // 
        DISPON      = 0x29, // 

        CASET       = 0x2A, // 
        PASET       = 0x2B, // 
        RAMWR       = 0x2C, // 
        RAMRD       = 0x2E, // 

        PTLAR       = 0x30, // 
        VSCRDEF     = 0x33, // 
        MADCTL      = 0x36, // 
        VSCRSADD    = 0x37, //      Vertical Scrolling Start Address
        PIXFMT      = 0x3A, //      COLMOD: Pixel Format Set

        RGB_INTERFACE = 0xB0, //      RGB Interface Signal Control
        FRMCTR1     = 0xB1, // 
        FRMCTR2     = 0xB2, // 
        FRMCTR3     = 0xB3, // 
        INVCTR      = 0xB4, // 
        DFUNCTR     = 0xB6, //      Display Function Control

        PWCTR1      = 0xC0, // 
        PWCTR2      = 0xC1, // 
        PWCTR3      = 0xC2, // 
        PWCTR4      = 0xC3, // 
        PWCTR5      = 0xC4, // 
        VMCTR1      = 0xC5, // 

        RDID1       = 0xDA, // 
        RDID2       = 0xDB, // 
        RDID3       = 0xDC, // 
        RDID4       = 0xDD, // 

        GMCTRP1     = 0xE0, // 
        GMCTRN1     = 0xE1, // 
        DGCTR1      = 0xE2, // 
        DGCTR2      = 0xE3, //
        
        CSCON       = 0xF0, //
        SPIRC       = 0xFB, //
    };
    
    void writeCmd(uint8_t cmd);
    void writeData(uint8_t data);
    void writeCmd(uint8_t cmd, uint8_t *data, int size);
    void writeCmd(uint8_t cmd, int size, ...);
    
    void setArea(int xStart, int yStart, int xEnd, int yEnd);
    
};

#endif