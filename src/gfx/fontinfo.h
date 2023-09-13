#ifndef _FONTINFO_H
#define _FONTINFO_H

#include <stdint.h>

#pragma pack(push,1)
class FontInfo
{
public:
    int width(char c) const {return letterInfo(c).ha;}
    int height() const {return h;}
    int ascent() const {return a;}
    int descent() const {return h - a;}
    int leftBearing(char c) const {return letterInfo(c).lb;}
    int rightBearing(char c) const {return letterInfo(c).rb;}
    int pixelSize() const {return ps;}
    bool bold() const {return b;}
    bool italic() const {return i;}
    const char *family() const {return ff;}
    
//    int width(const char *str) const
//    {
//        int w = 0;
//        while (*str)
//            w += width(*str++);
//        return w;
//    }
    
    int width(const char *str, int len=-1) const
    {
        int w = 0;
        while (*str && len--)
            w += width(*str++);
        return w;
    }
    
private:
    FontInfo() = delete;
    FontInfo(const FontInfo &) = delete;
    static const FontInfo *fromFontData(const uint32_t *fontdata)
    {
        return reinterpret_cast<const FontInfo*>(fontdata);
    }
    const uint32_t *fontData() const {return reinterpret_cast<const uint32_t *>(this);}
    friend class FontDatabase;
    friend class Display;
    
    uint8_t h;      // font height
    uint8_t a;      // font ascent (pixel count from top to baseline)
    uint8_t bpp;    // bits per pixel
    uint8_t fc;     // first char (e.g. ' ')
    uint8_t cc;     // char count
    uint8_t ps;     // pixel size
    union
    {
        uint8_t flags;
        struct
        {
            uint8_t b: 1;
            uint8_t i: 1;
        };
    };
    uint8_t _res;   // reserve

    char ff[16];    // font family

    struct LetterInfo
    {
        int8_t lb;  // left bearing
        int8_t rb;  // right bearing
        uint8_t ha; // horizontal advance
        uint8_t w;  // bitmap width = ha - lb - rb
        uint8_t sr; // start row with pixels
        uint8_t er; // end row with pixels
        uint16_t off; // offset in bytes to the letter bitmap
    } letters[];

    const LetterInfo &letterInfo(char c) const
    {
        int idx = (uint8_t)c - fc;
        if (idx >= cc)
            idx = 0;
        return letters[idx];
    }

    const uint8_t *bitmap() const
    {
        return reinterpret_cast<const uint8_t*>(letters + cc);
    }

    const uint8_t *bitmap(char c) const
    {
        return bitmap() + letterInfo(c).off;
    }
};
#pragma pack(pop)

#endif