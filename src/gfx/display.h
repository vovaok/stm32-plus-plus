#ifndef _DISPLAY
#define _DISPLAY

#include "font.h"
#include "color.h"
#include "rect.h"

class Image;
class FrameBuffer;

class Display
{
public:
    enum Orientation
    {
        Landscape          = 0,
        LandscapeHMirrored = 1,
        LandscapeVMirrored = 2,
        LandscapeInverted  = 3,
        Portrait           = 4,
        PortraitHMirrored  = 5,
        PortraitVMirrored  = 6,
        PortraitInverted   = 7
    };

    virtual void setOrientation(Orientation ori) {m_orientation = ori;}
    Orientation orientation() const {return m_orientation;}
    inline int width() const {return m_width;}
    inline int height() const {return m_height;}

    PixelFormat pixelFormat() const {return m_pixelFormat;}
    bool hasAlphaChannel() const;
    int bytesPerPixel() const {return m_bpp;}
    int bytesPerLine() const {return m_bpl;}
    int sizeInBytes() const {return m_bpl * m_height;}

    virtual void setPixel(int x, int y, uint32_t color) = 0;
    virtual uint32_t pixel(int x, int y) const {return m_bgColor;}
    virtual bool isReadable() const {return false;}

	void moveTo(int x, int y) {m_x = x; m_y = y;}
    int xPos() const {return m_x;}
    int yPos() const {return m_y;}
    void setColor(Color c) {m_color = toRgb(c);}
    Color color() const {return fromRgb(m_color);}
    void setBackgroundColor(Color c) {m_bgColor = toRgb(c);}
    Color backgroundColor() const {return fromRgb(m_bgColor);}
	void setFont(const Font &font) {m_font = font;}
    Font font() const {return m_font;}

	void drawString(int x, int y, const char *s);
    void drawString(int x, int y, int w, int h, int flags, const char *s);
    void drawString(const Rect &rect, int flags, const char *s);
    void drawLine(int x0, int y0, int x1, int y1);
    void drawRect(int x, int y, int w, int h);
    void drawRect(const Rect &rect);
    void drawFillRect(int x, int y, int w, int h);
    void drawFillRect(const Rect &rect);
    void drawCircle(int16_t x0, int16_t y0, int16_t r);
    void drawFillCircle(int16_t x0, int16_t y0, int16_t r);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
    void drawRoundRect(const Rect &rect, int r);
    void drawFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
    void drawFillRoundRect(const Rect &rect, int r);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
    void fillRoundRect(const Rect &rect, int r);
    void drawImage(int x, int y, const Image &img);
    void fillRect(int x, int y, int w, int h);
    void fillRect(const Rect &rect);
//    void fill(uint32_t color); //! @todo move it from FrameBuffer
    
    Color fromRgb(uint32_t rgb) const;
    uint32_t toRgb(Color color) const;

protected:
    int m_width = 0;
    int m_height = 0;
    Orientation m_orientation = Landscape; //! @todo move it to another class
    PixelFormat m_pixelFormat;
    int m_bpp = 0; // bytes per pixel
    int m_bpl = 0; // bytes per line

    Display() {}
    Display(int width, int height, PixelFormat pixelFormat=Format_RGB565);

    virtual void fillRect(int x, int y, int width, int height, uint32_t color) = 0;
    virtual void overlayRect(int x, int y, int width, int height, uint32_t color) = 0;
    virtual void copyRect(int x, int y, int width, int height, const uint8_t *buffer) = 0;
    virtual void blendRect(int x, int y, int width, int height, const uint8_t *buffer, PixelFormat format) = 0;
//    virtual void drawBuffer(int x, int y, FrameBuffer *fb) = 0;

    

private:
    uint16_t m_x=0, m_y=0;
    /*Color*/ uint32_t m_color = 0, m_bgColor = 0;
	Font m_font;

    void renderChar(char c, int &x, int &y);

    void drawPixel(int x, int y);
    void Draw_FastHLine(int x, int y, int length);
    void Draw_FastVLine(int x, int y, int length);
    void FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner, int16_t delta);
    void DrawCircle_Helper( int16_t x0, int16_t y0, int16_t r, uint8_t corner);
};

#endif
