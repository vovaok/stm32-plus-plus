#include "ili9486.h"

ILI9486::ILI9486(Spi *spi, Gpio::PinName cs, Gpio::PinName dc, Gpio::PinName rst) :
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
    m_spi->setBaudratePrescaler(1);
    m_spi->setDataSize(16);
    m_spi->setUseDmaTx(true);
//    m_spi->onBytesWritten = CLOSURE(m_cs, &Gpio::set);
    m_spi->open();
}

void ILI9486::setBacklightPin(Gpio::Config pin)
{
    m_pwmPin = pin;
    m_backlightPwm = new PwmOutput(pin);
    m_backlightPwm->setFrequency(60000);
    m_backlightPwm->setChannelInverted(pin, true);
    m_backlightPwm->setChannelEnabled(pin, true);
    m_backlightPwm->start();
}

void ILI9486::setBacklight(int percent)
{
    percent = BOUND(0, percent, 100);
    int pwm = lrintf(256*powf(1.057018f, percent)) - 1;
    if (m_backlightPwm)
        m_backlightPwm->setDutyCycle(m_pwmPin, pwm);
}

void ILI9486::init(Orientation ori)
{
    hardwareReset();
    initReg();

    setOrientation(ori);
//    delay(200);

    //sleep out
    LCD_WriteReg(0x11);
    delay(120);

    //Turn on the LCD display
    LCD_WriteReg(0x29);
}

void ILI9486::setOrientation(Orientation ori)
{
    if (static_cast<int>(ori) > 7)
        ori = Landscape;

    m_orientation = ori;

    static uint16_t orireg[8] =
        {0x2862, 0x2822, 0x2842, 0x2802, 0x0822, 0x0802, 0x0862, 0x0842};
    uint16_t reg = orireg[ori];
    uint8_t MemoryAccessReg_Data = reg >> 8; //addr:0x36
    uint8_t DisFunReg_Data = reg & 0xFF; //addr:0xB6

    if (reg & 0x2000)
    {
        m_width = LCD_WIDTH;
        m_height = LCD_HEIGHT;
    }
    else
    {
        m_width = LCD_HEIGHT;
        m_height = LCD_WIDTH;
    }

    // Set the read / write scan direction of the frame memory
    LCD_WriteReg(0xB6);
    LCD_WriteData(0X00);
    LCD_WriteData(DisFunReg_Data);

    LCD_WriteReg(0x36);
    LCD_WriteData(MemoryAccessReg_Data);
}

void ILI9486::hardwareReset()
{
    m_rst->reset();
    delay(10);
    m_rst->set();
    delay(50);
}

void ILI9486::initReg()
{
    LCD_WriteReg(0XF9);
    LCD_WriteData(0x00);
    LCD_WriteData(0x08);

    LCD_WriteReg(0xC0);
    LCD_WriteData(0x19);//VREG1OUT POSITIVE
    LCD_WriteData(0x1a);//VREG2OUT NEGATIVE

    LCD_WriteReg(0xC1);
    LCD_WriteData(0x45);//VGH,VGL    VGH>=14V.
    LCD_WriteData(0x00);

    LCD_WriteReg(0xC2);	//Normal mode, increase can change the display quality, while increasing power consumption
    LCD_WriteData(0x33);

    LCD_WriteReg(0XC5);
    LCD_WriteData(0x00);
    LCD_WriteData(0x28);//VCM_REG[7:0]. <=0X80.

    LCD_WriteReg(0xB1);//Sets the frame frequency of full color normal mode
    LCD_WriteData(0xA0);//0XB0 =70HZ, <=0XB0.0xA0=62HZ
    LCD_WriteData(0x11);

    LCD_WriteReg(0xB4);
    LCD_WriteData(0x02); //2 DOT FRAME MODE,F<=70HZ.

    LCD_WriteReg(0xB6);//
    LCD_WriteData(0x00);
    LCD_WriteData(0x42);//0 GS SS SM ISC[3:0];
    LCD_WriteData(0x3B);

    LCD_WriteReg(0xB7);
    LCD_WriteData(0x07);

    LCD_WriteReg(0xE0);
    LCD_WriteData(0x1F);
    LCD_WriteData(0x25);
    LCD_WriteData(0x22);
    LCD_WriteData(0x0B);
    LCD_WriteData(0x06);
    LCD_WriteData(0x0A);
    LCD_WriteData(0x4E);
    LCD_WriteData(0xC6);
    LCD_WriteData(0x39);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);

    LCD_WriteReg(0XE1);
    LCD_WriteData(0x1F);
    LCD_WriteData(0x3F);
    LCD_WriteData(0x3F);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x1F);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x46);
    LCD_WriteData(0x49);
    LCD_WriteData(0x31);
    LCD_WriteData(0x05);
    LCD_WriteData(0x09);
    LCD_WriteData(0x03);
    LCD_WriteData(0x1C);
    LCD_WriteData(0x1A);
    LCD_WriteData(0x00);

    LCD_WriteReg(0XF1);
    LCD_WriteData(0x36);
    LCD_WriteData(0x04);
    LCD_WriteData(0x00);
    LCD_WriteData(0x3C);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x0F);
    LCD_WriteData(0xA4);
    LCD_WriteData(0x02);

    LCD_WriteReg(0XF2);
    LCD_WriteData(0x18);
    LCD_WriteData(0xA3);
    LCD_WriteData(0x12);
    LCD_WriteData(0x02);
    LCD_WriteData(0x32);
    LCD_WriteData(0x12);
    LCD_WriteData(0xFF);
    LCD_WriteData(0x32);
    LCD_WriteData(0x00);

    LCD_WriteReg(0XF4);
    LCD_WriteData(0x40);
    LCD_WriteData(0x00);
    LCD_WriteData(0x08);
    LCD_WriteData(0x91);
    LCD_WriteData(0x04);

    LCD_WriteReg(0XF8);
    LCD_WriteData(0x21);
    LCD_WriteData(0x04);

    LCD_WriteReg(0X3A);	//Set Interface Pixel Format
    LCD_WriteData(0x55);
}

void ILI9486::LCD_WriteReg(uint8_t reg)
{
    m_dc->reset();
    m_cs->reset();
    m_spi->write16(reg);
    m_cs->set();
}

void ILI9486::LCD_WriteData(uint16_t data)
{
    m_dc->set();
    m_cs->reset();
    m_spi->write16(data);
    m_cs->set();
}

void ILI9486::setArea(int xStart, int yStart, int xEnd, int yEnd)
{
    //set the X coordinates
    LCD_WriteReg(0x2A);
    LCD_WriteData(xStart >> 8);	 				//Set the horizontal starting point to the high octet
    LCD_WriteData(xStart & 0xff);	 				//Set the horizontal starting point to the low octet
    LCD_WriteData(xEnd >> 8);	//Set the horizontal end to the high octet
    LCD_WriteData(xEnd & 0xff);	//Set the horizontal end to the low octet

    //set the Y coordinates
    LCD_WriteReg(0x2B);
    LCD_WriteData(yStart >> 8);
    LCD_WriteData(yStart & 0xff);
    LCD_WriteData(yEnd >> 8);
    LCD_WriteData(yEnd & 0xff);
}

void ILI9486::fillRect(int x, int y, int width, int height, uint16_t color)
{
    if (width == 0 || height == 0)
        return;
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
    LCD_WriteReg(0x2C);
    m_dc->set();
    m_cs->reset();
    int size = width * height;

//    while (size--)
//        LCD_WriteData(color);

    while (size)
    {
        int sz = size;
        if (sz > 0xFFFF)
            sz = 0xFFFF;
        m_spi->writeFill16(&color, sz);
        m_spi->waitForBytesWritten();
        size -= sz;
    }

    m_cs->set();
}

void ILI9486::copyRect(int x, int y, int width, int height, const uint16_t *buffer)
{
    setArea(x, y, x+width-1, y+height-1);
    // Memory write start
    LCD_WriteReg(0x2C);
    m_dc->set();
    m_cs->reset();
    int size = width * height;

    while (size)
    {
        int sz = size;
        if (sz > 32767) // max DMA number of data
            sz = 32767;
        m_spi->write((uint8_t*)buffer, sz*2);
        m_spi->waitForBytesWritten();
        size -= sz;
		buffer += sz;
    }

    m_cs->set();
}

void ILI9486::setPixel(int x, int y, uint16_t color)
{
    setArea(x, y, x, y);
    LCD_WriteReg(0x2C);
    LCD_WriteData(color);
}

//uint16_t ILI9486::pixel(int x, int y)
//{
//    return 0; // NE RABOTAET NA ETOM SRANOM KITE!!!
//}