#pragma once

#include <stdint.h>
#include "GUI/guitypes.h"
	
class Color
{
public:
	Color() : m_value(0) {}
	Color(uint8_t r, uint8_t g, uint8_t b) : m_value(RGB(r, g, b)) {}
	Color(StdColor c) : m_value(static_cast<uint16_t>(c)) {}
	
    static Color fromRgb565(uint16_t rgb)
    {
        return Color(static_cast<StdColor>(rgb));
    }
    
    uint8_t r() const {return m_value >> 11;} // 5-bit value
    uint8_t g() const {return (m_value >> 5) & 0x3F;} // 6-bit value
    uint8_t b() const {return m_value & 0x1F;} // 5-bit value
    
    uint8_t luminance() const {return 0;}
    
    uint16_t rgb565() const {return m_value;}
    
    static Color blend(Color fg, Color bg, uint8_t alpha)
    {
        uint8_t r = (fg.r() * alpha + bg.r() * (255 - alpha)) / 255;
        uint8_t g = (fg.g() * alpha + bg.g() * (255 - alpha)) / 255;
        uint8_t b = (fg.b() * alpha + bg.b() * (255 - alpha)) / 255;
        return Color::fromRgb565(RGB565(r, g, b));
    }

private:
    uint16_t m_value;
};
