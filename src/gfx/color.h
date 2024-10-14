#pragma once

#include <stdint.h>
#include "gui/guitypes.h"
	
//class Color
//{
//public:
//    Color() : m_value(0) {}
//	Color(uint8_t r, uint8_t g, uint8_t b) : m_value(RGB(r, g, b)) {}
//	Color(StdColor c) : m_value(static_cast<uint16_t>(c)) {}
////	Color() : m_value(0), m_alpha(255) {}
////	Color(uint8_t r, uint8_t g, uint8_t b) : m_value(RGB(r, g, b)), m_alpha(255) {}
////	Color(StdColor c) : m_value(static_cast<uint16_t>(c)), m_alpha(255) {}
////    Color(uint16_t rgb, uint8_t alpha) : m_value(rgb), m_alpha(alpha) {}
	
//    static Color fromRgb565(uint16_t rgb)
//    {
//        return Color(static_cast<StdColor>(rgb));
//    }
    
////    static Color transparent()
////    {
////        Color col;
////        col.m_alpha = 0;
////        return col;
////    }
    
//    uint8_t alpha() const {return 255;}//{return m_alpha;}
//    uint8_t r() const {return m_value >> 11;} // 5-bit value
//    uint8_t g() const {return (m_value >> 5) & 0x3F;} // 6-bit value
//    uint8_t b() const {return m_value & 0x1F;} // 5-bit value
    
//    uint8_t luminance() const {return 0;}
    
//    uint16_t rgb565() const {return m_value;}
    
//    static Color blend(Color fg, Color bg, uint8_t alpha)
//    {
//        uint8_t r = (fg.r() * alpha + bg.r() * (255 - alpha)) / 255;
//        uint8_t g = (fg.g() * alpha + bg.g() * (255 - alpha)) / 255;
//        uint8_t b = (fg.b() * alpha + bg.b() * (255 - alpha)) / 255;
////        uint8_t a = (fg.alpha() * alpha + bg.alpha() * (255 - alpha)) / 255;
//        return Color::fromRgb565(RGB565(r, g, b));
//    }
    
//    bool operator ==(const Color &other) const {return m_value == other.m_value;}// && m_alpha == other.m_alpha;}
//    bool operator !=(const Color &other) const {return !operator==(other);}

//private:
//    uint16_t m_value;
////    uint8_t m_alpha;
//};



// 32-bit color version:

class Color
{
public:
	Color() : m_value(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : m_value((a << 24) | (r << 16) | (g << 8) | (b)) {}
	Color(uint8_t r, uint8_t g, uint8_t b) : m_value((255 << 24) | (r << 16) | (g << 8) | (b)) {}
	Color(StdColor c) : m_value(static_cast<uint32_t>(c)) {}
    Color(uint32_t rgb) : m_value(rgb) {}
	
//    static Color fromRgb565(uint16_t rgb)
//    {
//        uint8_t r = (rgb >> 11) * 255L / 31;
//        uint8_t g = ((rgb >> 5) & 0x3F) * 255L / 63;
//        uint8_t b = (rgb & 0x1F) * 255L / 31;
//        return Color(r, g, b);
//    }
    
    uint8_t alpha() const {return m_value >> 24;}
    void setAlpha(uint8_t value) {m_value = (m_value & 0x00FFFFFF) | (value << 24);}
    uint8_t r() const {return m_value >> 16;}
    void setR(uint8_t value) {m_value = (m_value & 0xFF00FFFF) | (value << 16);}
    uint8_t g() const {return m_value >> 8;}
    void setG(uint8_t value) {m_value = (m_value & 0xFFFF00FF) | (value << 8);}
    uint8_t b() const {return m_value;}
    void setB(uint8_t value) {m_value = (m_value & 0xFFFFFF00) | (value);}

    uint32_t rgb() const {return m_value;}
    
//    uint8_t luminance() const {return 0;}
    
//    uint16_t rgb565() const
//    {
//        return ((m_value >> 8) & 0xF8) | ((m_value >> 5) & 0x7E) | ((m_value >> 3) & 0x1F);
//    }
    
    static Color blend(Color fg, Color bg, uint8_t alpha)
    {
        uint8_t fa = fg.alpha() * alpha / 255;
        uint8_t ba = bg.alpha();
        uint8_t amult = fa * ba / 255;
        uint8_t a = fa + ba - amult;
        if (!a)
            return 0;
        uint8_t r = (fg.r() * fa + bg.r() * (ba - amult)) / a;
        uint8_t g = (fg.g() * fa + bg.g() * (ba - amult)) / a;
        uint8_t b = (fg.b() * fa + bg.b() * (ba - amult)) / a;
        // premultiplied:
//        uint8_t a = (fa     + ba     * (255 - fa)) / 255;
//        uint8_t r = (fg.r() + bg.r() * (255 - fa)) / (255);
//        uint8_t g = (fg.g() + bg.g() * (255 - fa)) / (255);
//        uint8_t b = (fg.b() + bg.b() * (255 - fa)) / (255);
        return Color(r, g, b, a);
    }

    bool operator ==(const Color &other) const {return m_value == other.m_value;}
    bool operator !=(const Color &other) const {return !operator==(other);}

private:
    uint32_t m_value;
};
