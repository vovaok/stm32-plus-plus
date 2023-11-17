#include "fontdatabase.h"
#include "font.h"
#include <string.h>
#include <stdlib.h>

std::vector<const FontInfo *> FontDatabase::m_fonts;

void FontDatabase::addApplicationFontFromData(const uint32_t *fontdata)
{
    for (const FontInfo *fi: m_fonts)
    {
        if (fontdata == fi->fontData()) // already contains this font
            return;
    }

    m_fonts.push_back(FontInfo::fromFontData(fontdata));
}

Font FontDatabase::systemFont()
{
    if (!m_fonts.empty())
        return Font(m_fonts.at(0));
    return Font(static_cast<FontInfo *>(nullptr));
}

Font FontDatabase::font(const char *family, int pixelSize, bool bold, bool italic)
{
    Font font; // default system font
    int delta_size = 100;
    bool b_match = false;
    bool i_match = false;
    for (const FontInfo *fi: m_fonts)
    {
        if (!strstr(fi->ff, family))
            continue;

        int ds = abs(fi->ps - pixelSize);
        if (ds < delta_size)
        {
            delta_size = ds;
            b_match = i_match = false;
            font.m_fi = fi;
        }
        if (!b_match && bold == fi->b)
        {
            b_match = true;
            font.m_fi = fi;
        }
        if (!i_match && italic == fi->i)
        {
            i_match = true;
            font.m_fi = fi;
        }

        if (!delta_size && b_match && i_match) // exact match
            break;

    }
    return font;
}
