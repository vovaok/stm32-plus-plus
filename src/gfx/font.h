#ifndef _FONT
#define _FONT

#include "fontinfo.h"
#include "fontdatabase.h"

class Font
{
public:
    Font();
    Font(const char *family);
    Font(const char *family, int pixelSize, bool bold=false, bool italic=false);

    const FontInfo &info() const {return *m_fi;}

    inline bool operator==(const Font &other) {return m_fi == other.m_fi;}
    inline bool operator!=(const Font &other) {return m_fi != other.m_fi;}

private:
    const FontInfo *m_fi;

    Font(const FontInfo *fi) : m_fi(fi) {}
    friend class FontDatabase;
    friend class Display; // for renderChar
};

#endif