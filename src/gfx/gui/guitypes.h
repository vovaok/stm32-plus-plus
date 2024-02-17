#pragma once

#include <stdint.h>

//#define RGB565(r, g, b) (uint16_t)((b) | ((g)<<5) | ((r)<<11))
//#define RGB(r, g, b)	  RGB565((uint8_t)(r)>>3, (uint8_t)(g) >> 2, (uint8_t)(b) >> 3)
//#define RGBHEX(h)       RGB(((h) >> 16) & 0xFF, ((h) >> 8) & 0xFF, (h) & 0xFF)
//#define RED(c)          (((uint16_t)(c) >> 11) & 0x1F)
//#define GREEN(c)        (((uint16_t)(c) >> 5) & 0x3F)
//#define BLUE(c)         ((uint16_t)(c) & 0x1F)

#define RGB(r, g, b)    (0xFF000000 | ((uint8_t)(r)<<16) | ((uint8_t)(g) << 8) | (uint8_t)(b)) // 32-bit color
#define _MASK(bits)     ((1UL << (bits)) - 1)
#define RGB_COMP(rgb, offset, bits)   ((((rgb) >> (offset)) & _MASK(bits)) * 255L / _MASK(bits))
#define RGB_BITS(v, a, r, g, b)  ((((v) >> (32-(a)-(r)-(g)-(b))) & (_MASK(a) << ((r)+(g)+(b)))) \
                                | (((v) >> (24-(r)-(g)-(b))) & (_MASK(r) << ((g)+(b)))) \
                                | (((v) >> (16-(g)-(b))) & (_MASK(g) << (b))) \
                                | (((v) >> (8-(b))) & _MASK(b)))

enum StdColor //: uint32_t
{
    Transparent = 0,
	Black 	= RGB(0, 0, 0),
	Red 	= RGB(255, 0, 0),
	Yellow	= RGB(255, 255, 0),
	Green	= RGB(0, 255, 0),
	Cyan	= RGB(0, 255, 255),
	Blue 	= RGB(0, 0, 255),
	Magenta	= RGB(255, 0, 255),
	White	= RGB(255, 255, 255),
	Gray	= RGB(128, 128, 128),
    Orange  = RGB(255, 128, 0),
    Pink    = 0xFFFFB0BA,//(0xFFFC0FC0),
    DarkRed     = RGB(128, 0, 0),
    DarkYellow  = RGB(128, 128, 0),
    DarkGreen   = RGB(0, 128, 0),
    DarkCyan    = RGB(0, 128, 128),
    DarkBlue    = RGB(0, 0, 128),
    DarkMagenta = RGB(128, 0, 128),
    DarkGray    = RGB(64, 64, 64),
    LightGray   = RGB(192, 192, 192),
    DarkOrange  = RGB(128, 64, 0),
};

enum PixelFormat
{
    Format_ARGB8888 = 0,
    Format_RGB888   = 1,
    Format_RGB565   = 2,
    Format_ARGB1555 = 3,
    Format_ARGB4444 = 4,
    Format_L8       = 5, // 8-bit luminance
    Format_AL44     = 6, // 4-bit alpha, 4-bit luminance
    Format_AL88     = 7, // 8-bit alpha, 8-bit luminance
    Format_L4       = 8, // these modes used by DMA2D only!
    Format_A8       = 9, // these modes used by DMA2D only!
    Format_A4       = 10 // these modes used by DMA2D only!
};

enum Alignment
{
    AlignLeft       = 0x01,
    AlignRight      = 0x02,
    AlignHCenter    = 0x04,
    AlignJustify    = 0x08,
    AlignTop        = 0x10,
    AlignBottom     = 0x20,
    AlignVCenter    = 0x40,
    AlignCenter     = AlignHCenter | AlignVCenter,
    AlignHorizontal_Mask    = 0x0F,
    AlignVertical_Mask      = 0xF0
};

enum TextFlag
{
    TextWordWrap = 0x1000
};

enum Direction
{
    Horizontal,
    Vertical
};

enum Language
{
    AnyLanguage = 0,
    ru_RU,
    en_US,
};
