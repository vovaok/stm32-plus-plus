#include "st7735.h"
#include <math.h>
#include "gfx/image.h"

ST7735::ST7735(Spi *spi, Gpio::PinName cs, Gpio::PinName dc, Gpio::PinName rst) :
    Display(),
    m_spi(spi),
    m_backlightPwm(nullptr)
{
    m_cs = new Gpio(cs, Gpio::Output);
    m_cs->set();
    m_dc = new Gpio(dc, Gpio::Output);
    m_rst = new Gpio(rst, Gpio::Output);
    m_rst->set();

    m_spi->setMasterMode();
    m_spi->setCPOL_CPHA(0, 0);
    m_spi->setBaudrate(m_spiBaudrate);
//    m_spi->setBaudrate(10000000);
    m_spi->setDataSize(16);
    m_spi->setUseDmaTx(false);
    m_spi->setUseDmaRx(false);
//    m_spi->onBytesWritten = CLOSURE(m_cs, &Gpio::set);
    m_spi->open();
}

void ST7735::setBacklightPin(Gpio::Config pin)
{
    m_pwmPin = pin;
    m_backlightPwm = new PwmOutput(pin);
    m_backlightPwm->setFrequency(60000);
//    m_backlightPwm->setChannelInverted(pin, true);
    m_backlightPwm->setChannelEnabled(pin, true);
    m_backlightPwm->start();
}

void ST7735::setBacklight(int percent)
{
    percent = BOUND(0, percent, 100);
    int pwm = lrintf(256*powf(1.057018f, percent)) - 1;
    if (m_backlightPwm)
        m_backlightPwm->setDutyCycle(m_pwmPin, pwm);
}

void ST7735::init(Orientation ori)
{
    hardwareReset();
    
    uint8_t id[4] {0};
    
//    m_spi->setDataSize(8);
//    m_cs->reset();
//    m_dc->reset();
//    m_spi->write(0xdb);
//    m_dc->set();
////    for (int i=0; i<4; i++)
//        id[0] = m_spi->read();
//    m_cs->set();
    
    m_spi->setDataSize(8);
    
    initReg();
    
    m_bpp = 16;

    setOrientation(ori);
//    delay(200);

    writeCmd(SLPOUT);
    delay(200);
    // Display on
    writeCmd(DISPON);
    delay(10);
    
    m_spi->setDataSize(16);
    
    setBackgroundColor(White);
    setColor(Black);
    Display::fillRect(0, 0, m_width, m_height);
    drawString(0, 0, m_width, m_height, AlignCenter, "Display initialized");
    delay(10);
    
//    m_spi->close();
//    m_spi->setUseDmaTx(true);
//    m_spi->open();
}

void ST7735::setOrientation(Orientation ori)
{
    if (static_cast<int>(ori) > 7)
        ori = Landscape;

    m_orientation = ori;

    uint8_t mad = (1<<3);
    
    switch (ori)
    {
    case Portrait: mad |= (1<<6); break;
    case Landscape: mad |= (1<<5); break;
    case PortraitInverted: mad |= (1<<7); break;
    case LandscapeInverted: mad |= (1<<5)|(1<<6)|(1<<7); break;
    }
    
    writeCmd(MADCTL);
    writeData(mad);
        
    if (ori < Portrait)
    {
        m_width = LCD_WIDTH;
        m_height = LCD_HEIGHT;
    }
    else
    {
        m_width = LCD_HEIGHT;
        m_height = LCD_WIDTH;
    }
    m_bpl = m_width * 2;
    
//    static uint16_t orireg[8] =
//        {0x2862, 0x2822, 0x2842, 0x2802, 0x0822, 0x0802, 0x0862, 0x0842};
//    uint16_t reg = orireg[ori];
//    uint8_t MemoryAccessReg_Data = reg >> 8; //addr:0x36
//    uint8_t DisFunReg_Data = reg & 0xFF; //addr:0xB6
//
//    if (reg & 0x2000)
//    {
//        m_width = LCD_WIDTH;
//        m_height = LCD_HEIGHT;
//    }
//    else
//    {
//        m_width = LCD_HEIGHT;
//        m_height = LCD_WIDTH;
//    }
//
//    // Set the read / write scan direction of the frame memory
//    writeCmd(0xB6);
//    writeData(0x00);
//    writeData(DisFunReg_Data);
//
//    writeCmd(0x36);
//    writeData(MemoryAccessReg_Data);
}

void ST7735::hardwareReset()
{
    m_rst->reset();
    delay(10);
    m_rst->set();
    delay(50);
}

void ST7735::initReg()
{
    delay(1);
    writeCmd(SWRESET);
    delay(5);

//    // unlock something...
//    // Command Set Control - enable command 2 part I
    writeCmd(0xF0, (uint8_t *)"\xC3", 1);
//    // Command Set Control - enable command 2 part II
    writeCmd(0xF0, (uint8_t *)"\x96", 1);

//    writeCmd(0x36, 1, 0x48);
//    writeCmd(0x3A, 1, 0x05);              //Interlace Pixel Format [XX]
//    writeCmd(0xB4, 1, 0x01);              //Inversion Control [01]
//    writeCmd(0xB6, 3, 0x80, 0x02, 0x3B);  // Display Function Control [80 02 3B]
//    writeCmd(0xE8, 8, 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33);   //Adjustment Control 3 [40 8A 00 00 25 0A 38 33]
//    writeCmd(0xC1, 1, 0x06);              //Power Control 2 [13]
//    writeCmd(0xC2, 1, 0xA7);              //Power Control 3 [A?]
//    writeCmd(0xC5, 1, 0x18);              //VCOM=0.9 [1C]
//    //0x11, 0x80,                 //delay 150 ms
//    writeCmd(0xE0, 14, 0xF0, 0x09, 0x0B, 0x06, 0x04, 0x15, 0x2F, 0x54, 0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B);     //PVGAMCTRL: Positive Voltage Gamma control
//    writeCmd(0xE1, 14, 0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B, 0x43, 0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B);     //NVGAMCTRL: Negative Voltage Gamma control
//    
    writeCmd(MADCTL, (uint8_t *)"\x08", 1);
    writeCmd(PIXFMT, (uint8_t *) "\x05", 1);
    m_pixelFormat = Format_RGB565;
    
    // Interface Mode Control
  //  writeCmd(0xB0, (uint8_t *) "\x80", 1);
    // Display Function Control
//    writeCmd(0xB6, (uint8_t *) "\x20\x02", 2);
    // Blanking Porch Control
    writeCmd(0xB5, (uint8_t *) "\x02\x03\x00\x04", 4);

    // Frame Control
    writeCmd(FRMCTR1, (uint8_t *)"\x80\x10", 2);

    // Display Inversion Control
    writeCmd(INVCTR, (uint8_t *)"\x00", 1);

    // Entry Mode Set
    writeCmd(0xB7, (uint8_t *)"\xC6", 1);

    // VCOM Control
    writeCmd(VMCTR1, (uint8_t *)"\x24", 1);

    // VCOM Control
    writeCmd(0xE4, (uint8_t *)"\x31", 1);

    // Display Output Ctrl Adjust
    writeCmd(0xE8, (uint8_t *) "\x40\x8A\x00\x00\x29\x19\xA5\x33", 8);

    // Power Control 3
    writeCmd(PWCTR3, (uint8_t *)"\xA7", 1);

    // Positive gamma control
    writeCmd(GMCTRP1,
          (uint8_t *)"\x09\x16\x09\x20\x21\x1B\x13\x19\x17\x15\x1E\x2B\x04\x05\x02\x0E", 16);

    // Negative gamma control
    writeCmd(GMCTRN1,
          (uint8_t *)"\x0B\x14\x08\x1E\x22\x1D\x18\x1E\x1B\x1A\x24\x2B\x06\x06\x02\x0F", 16);
    
//    // Positive gamma control
//    writeCmd(GMCTRP1,
//          (uint8_t *)"\xD0\x08\x0F\x06\x06\x33\x30\x33\x47\x17\x13\x13\x2B\x31", 14);
//
//    // Negative gamma control
//    writeCmd(GMCTRN1,
//          (uint8_t *)"\xD0\x0A\x11\x0B\x09\x07\x2F\x33\x47\x38\x15\x16\x2C\x32", 14);

    // writeCmd(RGB_INTERFACE, (uint8_t *)"\x00", 1); // RGB mode off (0xB0)

    writeCmd(MADCTL);
    writeData(0x48);//MAD_DATA_RIGHT_THEN_DOWN);

    // Normal display on
    writeCmd(NORON);
    // Display inversion off
    writeCmd(INVON);
    
//    // lock something...
//    // Command Set Control - disable command 2 part I
    writeCmd(0xF0, (uint8_t *)"\x3Ñ", 1);
//    // Command Set Control - disable command 2 part II
    writeCmd(0xF0, (uint8_t *)"\x69", 1);
    
//    writeCmd(0x36, 1, (1<<3)|(1<<5)); // set direction
//    
//    setArea(0, 0, 200, 200);
//    writeCmd(0x2C);
//    
//    m_spi->setDataSize(16);
//    
//    m_cs->reset();
//        m_dc->set();
//    for (int i=0; i<200*200; i++)
//    {
//        m_spi->write16(0x1f);
//    }
//    m_cs->set();
}

void ST7735::writeCmd(uint8_t cmd)
{
    m_cs->reset();
    m_dc->reset();
    m_spi->write(cmd);
    m_cs->set();
}

void ST7735::writeData(uint8_t data)
{    
    m_cs->reset();
    m_dc->set();
    m_spi->write(data);
    m_cs->set();
}

void ST7735::writeCmd(uint8_t cmd, uint8_t *data, int size)
{
    writeCmd(cmd);
    while (size--)
        writeData(*data++);
}

void ST7735::writeCmd(uint8_t cmd, int size, ...)
{
    writeCmd(cmd);
    va_list vl;
    va_start(vl, size);
    while (size--)
        writeData(va_arg(vl, uint8_t));
    va_end(vl);
}

void ST7735::setArea(int xStart, int yStart, int xEnd, int yEnd)
{
//    m_spi->setDataSize(16);
    m_cs->reset();
    m_dc->reset();
    m_spi->write16(0x2A);
    m_dc->set();
    m_spi->write16(xStart);
    m_spi->write16(xEnd);
    m_dc->reset();
    m_spi->write16(0x2B);
    m_dc->set();
    m_spi->write16(yStart);
    m_spi->write16(yEnd);
    m_cs->set();
}

void ST7735::readRect(int x, int y, int w, int h, uint8_t *buffer)
{
    setArea(x, y, x+w-1, y+h-1);

//    m_spi->setDataSize(16);
    
    m_dc->reset();
    m_cs->reset();
    // Memory read start
    m_spi->write16(RAMRD << 8);
    m_dc->set();
    int size = w * h * 2;
    
    m_spi->setBaudrate(10000000);
    
//    uint16_t *dst = reinterpret_cast<uint16_t *>(buffer);
    while (size)
    {
        int sz = size;
        if (sz > 32767*2)
            sz = 32767*2;
//        sz = 1;
//        *dst++ = m_spi->read16();
        m_spi->read(buffer, sz);
        m_spi->waitForBytesWritten();
        buffer += sz;
        size -= sz;
    }

    m_cs->set();
    
    m_spi->setBaudrate(m_spiBaudrate);
}

void ST7735::fillRect(int x, int y, int width, int height, uint32_t color)
{
    if (width == 0 || height == 0)
        return;
    if (width == 1 && height == 1)
    {
        setPixel(x, y, color);
        return;
    }
    if (width < 0)
    {
        x += width;
        width = -width;
    }
    if (height < 0)
    {
        y += height;
        height = -height;
    }
    setArea(x, y, x+width-1, y+height-1);
    // Memory write start
    m_cs->reset();
    m_dc->reset();
    m_spi->write16(RAMWR);
    m_dc->set();
    int size = width * height;

//    while (size--)
//        writeData(color);

//    m_spi->setDataSize(16);
    m_fillColor = color;
    while (size)
    {
        int sz = size;
        if (sz > 0xFFFF)
            sz = 0xFFFF;
        m_spi->writeFill16(&m_fillColor, sz);
        m_spi->waitForBytesWritten();
        size -= sz;
    }

    m_cs->set();
}

void ST7735::copyRect(int x, int y, int width, int height, const uint8_t *buffer)
{
    setArea(x, y, x+width-1, y+height-1);
    // Memory write start
    m_cs->reset();
    m_dc->reset();
    m_spi->write16(RAMWR);
    m_dc->set();
    int size = width * height;

//    m_spi->setDataSize(16);
    while (size)
    {
        int sz = size;
        if (sz > 32767) // max DMA number of data
            sz = 32767;
        m_spi->write((uint8_t*)buffer, sz*2);
        m_spi->waitForBytesWritten();
        size -= sz;
		buffer += sz*2;
    }

    m_cs->set();
}

void ST7735::overlayRect(int x, int y, int width, int height, uint32_t color)
{
    fillRect(x, y, width, height, color); // no blending
}

void ST7735::blendRect(int x, int y, int width, int height, const uint8_t *buffer, PixelFormat format)
{
    Image fb((const char *)buffer, width, height, format);
    Image img(width, height, m_pixelFormat);
//    img.fill(backgroundColor());
    readRect(x, y, width, height, img.data());
    img.drawImage(0, 0, fb); 
    drawBuffer(x, y, &img);
//    THROW(Exception::BadSoBad); // not implemented
}

void ST7735::drawBuffer(int x, int y, const FrameBuffer *fb, int sx, int sy, int sw, int sh)
{
    if (x >= m_width || y >= m_height)
        return; // nothing to do

    if (x < 0)
    {
        sx -= x;
        x = 0;
    }
    if (y < 0)
    {
        sy -= y;
        y = 0;
    }

    if (sw <= 0)
        sw = fb->width();
    if (sh <= 0)
        sh = fb->height();

    if (x + sw > m_width)
        sw = m_width - x;
    if (y + sh > m_height)
        sh = m_height - y;

    if (sx >= fb->width() || sy >= fb->height())
        return; // nothing to do

    Color fg;
    PixelFormat format = fb->pixelFormat();
    
    bool doCopy = (format == pixelFormat());
    if (doCopy && sw == fb->width())
    {
        copyRect(x, y, sw, sh, fb->scanLine(sy));
    }
    else for (int i=0; i<sh; i++)
    {
        int yi = y + i;
        int ys = sy + i;
        if (doCopy)
        {
            copyRect(x, yi, sw, 1, fb->scanLine(ys) + sx*(m_bpp>>3));
        }
        else for (int j=0; j<sw; j++)
        {
            int xj = x + j;
            int xs = sx + j;
            if (format == Format_A8 || format == Format_A4)
            {
                fg = color();
                fg.setAlpha(fb->pixel(xs, ys) * fb->opacity() / 255);
            }
            else
            {
                fg = fb->fromRgb(fb->pixel(xs, ys));
                fg.setAlpha(fg.alpha() * fb->opacity() / 255);
            }
            Color bg = fromRgb(pixel(xj, yi));
            setPixel(xj, yi, toRgb(Color::blend(fg, bg, 255)));
        }
    }
}

void ST7735::setPixel(int x, int y, uint32_t color)
{   
    m_cs->reset();
    m_dc->reset();
    m_spi->write16(0x2A);
    m_dc->set();
    m_spi->write16(x);
//    m_spi->write16(x);
    m_dc->reset();
    m_spi->write16(0x2B);
    m_dc->set();
    m_spi->write16(y);
//    m_spi->write16(y);
    m_dc->reset();
    m_spi->write16(0x2C);
    m_dc->set();
    m_spi->write16(color);
    m_cs->set();
}

uint32_t ST7735::pixel(int x, int y) const
{
    uint32_t color;
    m_cs->reset();
    m_dc->reset();
    m_spi->write16(0x2A);
    m_dc->set();
    m_spi->write16(x);
//    m_spi->write16(x);
    m_dc->reset();
    m_spi->write16(0x2B);
    m_dc->set();
    m_spi->write16(y);
//    m_spi->write16(y);
    m_dc->reset();
    m_spi->write16(0x2E << 8);
    m_dc->set();
    m_spi->setBaudrate(10000000);
    color = m_spi->read16();
    m_spi->setBaudrate(m_spiBaudrate);
    m_cs->set();
    return color;
}

Color ST7735::toColorRGB(char r,char g, char b, char a)
{

  return Color(255-b, 255-g,255-r,a);
  
}