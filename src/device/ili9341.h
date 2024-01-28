#pragma once

#include "gfx/lcddisplay.h"
#include "spi.h"

class ILI9341 : public LcdDisplay
{
public:
    ILI9341(Spi *spi, Gpio::PinName cs, Gpio::PinName dc);

    void init();

private:
    Spi *m_spi;
    Gpio *m_cs, *m_dc;

//    void hardwareReset();
    void delay(int ms) {for (int w=ms*20000; --w;);}
    void initReg();
    void LCD_WriteCommand(uint8_t reg);
    void LCD_WriteData(uint8_t data);

    enum Register
    {
        LCD_SLEEP_OUT       = 0x11, // Sleep out register
        LCD_GAMMA           = 0x26, // Gamma register
        LCD_DISPLAY_OFF     = 0x28, // Display off register
        LCD_DISPLAY_ON      = 0x29, // Display on register
        LCD_COLUMN_ADDR     = 0x2A, // Colomn address register
        LCD_PAGE_ADDR       = 0x2B, // Page address register
        LCD_GRAM            = 0x2C, // GRAM register
        LCD_MAC             = 0x36, // Memory Access Control register
        LCD_PIXEL_FORMAT    = 0x3A, // Pixel Format register
        LCD_WDB             = 0x51, // Write Brightness Display register
        LCD_WCD             = 0x53, // Write Control Display register
        LCD_RGB_INTERFACE   = 0xB0, // RGB Interface Signal Control
        LCD_FRC             = 0xB1, // Frame Rate Control register
        LCD_BPC             = 0xB5, // Blanking Porch Control register
        LCD_DFC             = 0xB6, // Display Function Control register
        LCD_POWER1          = 0xC0, // Power Control 1 register
        LCD_POWER2          = 0xC1, // Power Control 2 register
        LCD_VCOM1           = 0xC5, // VCOM Control 1 register
        LCD_VCOM2           = 0xC7, // VCOM Control 2 register
        LCD_POWERA          = 0xCB, // Power control A register
        LCD_POWERB          = 0xCF, // Power control B register
        LCD_PGAMMA          = 0xE0, // Positive Gamma Correction register
        LCD_NGAMMA          = 0xE1, // Negative Gamma Correction register
        LCD_DTCA            = 0xE8, // Driver timing control A
        LCD_DTCB            = 0xEA, // Driver timing control B
        LCD_POWER_SEQ       = 0xED, // Power on sequence register
        LCD_3GAMMA_EN       = 0xF2, // 3 Gamma enable register
        LCD_INTERFACE       = 0xF6, // Interface control register
        LCD_PRC             = 0xF7, // Pump ratio control register
    };
};