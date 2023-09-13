#pragma once

#define RGB565(r, g, b) (uint16_t)((b) | ((g)<<5) | ((r)<<11))
#define RGB(r, g, b)	RGB565((uint8_t)(r)>>3, (uint8_t)(g) >> 2, (uint8_t)(b) >> 3)
//#define RED(c)          (((uint16_t)(c) >> 11) & 0x1F)
//#define GREEN(c)        (((uint16_t)(c) >> 5) & 0x3F)
//#define BLUE(c)         ((uint16_t)(c) & 0x1F)

//#define RGB(r, g, b)    (((uint8_t)(r)<<16) | ((uint8_t)(g) << 8) | (uint8_t)(b)) // 32-bit color

enum StdColor: uint16_t
{
	Black 	= RGB(0, 0, 0),
	Red 	= RGB(255, 0, 0),
	Yellow	= RGB(255, 255, 0),
	Green	= RGB(0, 255, 0),
	Cyan	= RGB(0, 255, 255),
	Blue 	= RGB(0, 0, 255),
	Magenta	= RGB(255, 0, 255),
	White	= RGB(255, 255, 255),
	Gray	= RGB(128, 128, 128)	
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
