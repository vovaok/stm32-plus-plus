#ifndef _FONTDATABASE_H
#define _FONTDATABASE_H

#include "fontinfo.h"
#include <vector>

class Font;

class FontDatabase
{
public:
//    FontDatabase();
    
    Font font(const char *family, int pixelSize=12, bool bold=false, bool italic=false);
    
    static void addApplicationFontFromData(const uint32_t *fontdata);
    static Font systemFont();

private:
    static std::vector<const FontInfo *> m_fonts;
    
};

#endif