#include "font.h"

Font::Font()
{
    m_fi = FontDatabase::systemFont().m_fi;
}

Font::Font(const char *family)
{
    int ps = FontDatabase::systemFont().m_fi->pixelSize();
    FontDatabase fdb;
    m_fi = fdb.font(family, ps).m_fi;
}

Font::Font(const char *family, int pixelSize, bool bold, bool italic)
{
    FontDatabase fdb;
    m_fi = fdb.font(family, pixelSize, bold, italic).m_fi;
}
