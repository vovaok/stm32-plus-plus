#include "display.h"
#include "image.h"

void Display::drawString(int x, int y, const char *s)
{
    const FontInfo *fi = m_font.m_fi;
    if (!fi)
        return;

	while (*s)
		renderChar(*s++, x, y);
}

void Display::drawString(int x, int y, int w, int h, int flags, const char *s)
{
    const FontInfo *fi = m_font.m_fi;
    if (!fi)
        return;

    struct Line
    {
        const char *s;
        int w;
        int len;
        int wcnt; // word count
    };

    std::vector<Line> lines;

    const char *line = s;

    do
    {
        int linew = 0;
        int len = 0;
        int wcnt = 0;

        const char *word = line;
        bool flag = true;
        do
        {
            const char *nextword = nullptr;
            if (flags & TextWordWrap)
                nextword = strpbrk(word, " \t\n");
            else
                nextword = strchr(word, '\n');
            if (!nextword)
                nextword = line + strlen(line);

            flag = *nextword && *nextword != '\n';

//            len = nextword - line;
            int lw = fi->width(line, nextword - line);
            if (lw <= w || !wcnt)
            {
                linew = lw;
                len = nextword - line;
                wcnt++;
                if (*nextword)
                    word = nextword + 1;
                else
                    word = nullptr;
            }

            if (lw > w)
                break;

            if (!flag)
                wcnt = 0;
        }
        while (flag);

        lines.push_back({line, linew, len, wcnt});
        line = word;
    } while (line);

    int th = lines.size() * fi->height();
    int ypos = y + fi->ascent();
    if (flags & AlignBottom)
        ypos += h - th;
    else if (flags & AlignVCenter)
        ypos += (h - th) / 2;

    for (Line &ln: lines)
    {
        int xpos = x;
        if (flags & AlignRight)
            xpos += w - ln.w;
        else if (flags & AlignHCenter)
            xpos += (w - ln.w) / 2;

        int space = ((flags & AlignJustify) && ln.wcnt > 1)? (w - ln.w): 0;
        int wcnt = (ln.wcnt);// - 1);

        const char *s = ln.s;
        for (int i=0; i<ln.len; i++)
        {
            if (*s == ' ' || *s == '\t')
            {
                int sp = space / --wcnt;
                xpos += sp;
                space -= sp;
            }
            renderChar(*s++, xpos, ypos);
        }

        ypos += fi->height();
        if (ypos >= y + h)
            break;
    }

}

void Display::drawString(const Rect &rect, int flags, const char *s)
{
    drawString(rect.x(), rect.y(), rect.width(), rect.height(), flags, s);
}

void Display::renderChar(char c, int &x, int &y)
{
	const FontInfo *fi = m_font.m_fi;
    if (!fi)
        return;

	const FontInfo::LetterInfo &li = fi->letterInfo(c);
	const uint8_t *src = fi->bitmap(c);
	int x0 = m_x + x + li.lb;
    int y0 = m_y + y - fi->a;

    if (isReadable() && m_pixelFormat < 8 && fi->bpp == 4)
    {
        // li.w must be even for 4 bpp!
        blendRect(x0, y0 + li.sr, li.w, li.er - li.sr, src, Format_A4);
    }
    else if (isReadable() && m_pixelFormat < 8 && fi->bpp == 8)
    {
        blendRect(x0, y0 + li.sr, li.w, li.er - li.sr, src, Format_A8);
    }
    else
    {
        int bit = 0;
        uint8_t mask = (1 << fi->bpp) - 1;
        for (int r=li.sr; r<li.er; r++)
        {
            for (int c=0; c<li.w; c++)
            {
                const uint16_t *srcw = reinterpret_cast<const uint16_t*>(src);
                uint8_t v = ((*srcw >> bit) & mask);
                bit += fi->bpp;
                if (bit > 8)
                {
                    bit -= 8;
                    src++;
                }
                Color bgcolor = fromRgb(pixel(x0+c, y0+r));
                Color fgcolor = fromRgb(m_color);
                Color col = Color::blend(fgcolor, bgcolor, v * 255 / mask);
                setPixel(x0+c, y0+r, toRgb(col));
            }
        }
	}
	x += li.ha;
}

//void Display::drawImage(int x, int y, const Image &img)
//{
//    if (!img.isNull())
//    {
//        if (img.m_pixelFormat == m_pixelFormat && !img.hasAlphaChannel())
//            copyRect(m_x+x, m_y+y, img.width(), img.height(), img.data());
//        else
//            blendRect(m_x+x, m_y+y, img.width(), img.height(), img.data(), img.pixelFormat());
//    }
//}

void Display::drawImage(int x, int y, const Image &img, int sx, int sy, int sw, int sh)
{
    if (!img.isNull())
        drawBuffer(m_x+x, m_y+y, &img, sx, sy, sw, sh);
}

void Display::drawLine(int x0, int y0, int x1, int y1)
{
    if (x0 == x1)
    {
        if (y0 > y1)
            std::swap(y0, y1);
        Draw_FastVLine(x0, y0, y1 - y0 + 1);
    }
    else if (y0 == y1)
    {
        if (x0 > x1)
            std::swap(x0, x1);
        Draw_FastHLine(x0, y0, x1 - x0 + 1);
    }
    else
    {
        int16_t steep = abs(y1 - y0) > abs(x1 - x0);

        if (steep)
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        int16_t dx, dy;
        dx = x1 - x0;
        dy = abs(y1 - y0);

        int16_t err = dx / 2;
        int16_t ystep;

        if (y0 < y1)
            ystep = 1;
        else
            ystep = -1;

        for(; x0<=x1; x0++)
        {
            if (steep)
                drawPixel(y0, x0);
            else
                drawPixel(x0, y0);
            err -= dy;
            if(err < 0)
            {
                y0 += ystep;
                err += dx;
            }
        }
    }
}

void Display::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

void Display::drawRect(int x, int y, int w, int h)
{
    Draw_FastHLine(x, y, w);
    Draw_FastHLine(x, y+h-1, w);
    Draw_FastVLine(x, y, h);
    Draw_FastVLine(x+w-1, y, h);
}

void Display::drawRect(const Rect &rect)
{
    drawRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void Display::fillRect(int x, int y, int w, int h)
{
    if (hasAlphaChannel())
        overlayRect(m_x+x, m_y+y, w, h, m_bgColor);
    else
        fillRect(m_x+x, m_y+y, w, h, m_bgColor);
}

void Display::fillRect(const Rect &rect)
{
    if (hasAlphaChannel())
        overlayRect(m_x+rect.x(), m_y+rect.y(), rect.width(), rect.height(), m_bgColor);
    else
        fillRect(m_x+rect.x(), m_y+rect.y(), rect.width(), rect.height(), m_bgColor);
}

void Display::fillCircle(int x0, int y0, int r)
{
    uint32_t temp = m_color;
    m_color = m_bgColor;
    Draw_FastVLine(x0, y0-r, 2*r+1);
    FillCircle_Helper(x0, y0, r, 3, 0);
    m_color = temp;
}

Color Display::fromRgb(uint32_t rgb) const
{
    switch (m_pixelFormat)
    {
//        case Format_ARGB8888:   return Color(rgb); // this is default case
        case Format_RGB888:     return Color(0xFF000000 | rgb);
        case Format_RGB565:     return Color(RGB_COMP(rgb, 11, 5), RGB_COMP(rgb, 5, 6), RGB_COMP(rgb, 0, 5));
        case Format_ARGB1555:   return Color(RGB_COMP(rgb, 10, 5), RGB_COMP(rgb, 5, 5), RGB_COMP(rgb, 0, 5), (rgb >> 15) * 255);
        case Format_ARGB4444:   return Color(RGB_COMP(rgb, 8, 4), RGB_COMP(rgb, 4, 4), RGB_COMP(rgb, 0, 4), RGB_COMP(rgb, 12, 4));
//        case Format_L8:         m_bpp = 8; break;
//        case Format_AL44:       m_bpp = 8; break;
//        case Format_AL88:       m_bpp = 16; break;
        case Format_A4:         return Color(rgb << 28);
        case Format_A8:         return Color(rgb << 24);
//        case Format_L4:         m_bpp = 4; break;
        default: return Color(rgb);
    }
}

uint32_t Display::toRgb(Color color) const
{
    uint32_t rgb = color.rgb();
    switch (m_pixelFormat)
    {
//        case Format_ARGB8888:   return rgb; // this is default case
//        case Format_RGB888:     return rgb;
        case Format_RGB565:     return RGB_BITS(rgb, 0, 5, 6, 5);
        case Format_ARGB1555:   return RGB_BITS(rgb, 1, 5, 5, 5);
        case Format_ARGB4444:   return RGB_BITS(rgb, 4, 4, 4, 4);
//        case Format_L8:         m_bpp = 8; break;
//        case Format_AL44:       m_bpp = 8; break;
//        case Format_AL88:       m_bpp = 16; break;
        case Format_A4:         return rgb >> 28; break;
        case Format_A8:         return rgb >> 24;
//        case Format_L4:         m_bpp = 4; break;
        default: return rgb;
    }
}

void Display::drawFillRect(int x, int y, int w, int h)
{
    drawRect(x, y, w, h);
    fillRect(x+1, y+1, w-2, h-2);
}

void Display::drawFillRect(const Rect &rect)
{
    drawFillRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void Display::drawFillCircle(int16_t x0, int16_t y0, int16_t r)
{
    fillCircle(x0, y0, r-1);
    drawCircle(x0, y0, r);
}

void Display::drawCircle(int16_t x0, int16_t y0, int16_t r)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0  , y0+r);
  drawPixel(x0  , y0-r);
  drawPixel(x0+r, y0  );
  drawPixel(x0-r, y0  );

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y);
    drawPixel(x0 - x, y0 + y);
    drawPixel(x0 + x, y0 - y);
    drawPixel(x0 - x, y0 - y);
    drawPixel(x0 + y, y0 + x);
    drawPixel(x0 - y, y0 + x);
    drawPixel(x0 + y, y0 - x);
    drawPixel(x0 - y, y0 - x);
    }
}

void Display::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
    // smarter version
    Draw_FastHLine(x+r  , y    , w-2*r); // Top
    Draw_FastHLine(x+r  , y+h-1, w-2*r); // Bottom
    Draw_FastVLine(x    , y+r  , h-2*r); // Left
    Draw_FastVLine(x+w-1, y+r  , h-2*r); // Right
    // draw four corners
    DrawCircle_Helper(x+r    , y+r    , r, 1);
    DrawCircle_Helper(x+w-r-1, y+r    , r, 2);
    DrawCircle_Helper(x+w-r-1, y+h-r-1, r, 4);
    DrawCircle_Helper(x+r    , y+h-r-1, r, 8);
}

void Display::drawRoundRect(const Rect &rect, int r)
{
    drawRoundRect(rect.x(), rect.y(), rect.width(), rect.height(), r);
}

void Display::drawFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
    fillRoundRect(x+1, y+1, w-2, h-2, r-1);
    drawRoundRect(x, y, w, h, r);
}

void Display::drawFillRoundRect(const Rect &rect, int r)
{
    drawFillRoundRect(rect.x(), rect.y(), rect.width(), rect.height(), r);
}

void Display::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
    if (r > w/2)
        r = w/2;
    if (r > h/2)
        r = h/2;
    uint32_t temp = m_color;
    m_color = m_bgColor;
    fillRect(x+r, y, w-2*r, h);
    FillCircle_Helper(x+w-r-1, y+r, r, 1, h-2*r-1);
    FillCircle_Helper(x+r, y+r, r, 2, h-2*r-1);
    m_color = temp;
}

void Display::fillRoundRect(const Rect &rect, int r)
{
    fillRoundRect(rect.x(), rect.y(), rect.width(), rect.height(), r);
}

void Display::drawPixel(int x, int y)
{
    int rx = m_x + x;
    int ry = m_y + y;
    if (hasAlphaChannel())
    {
        Color bg = fromRgb(pixel(rx, ry));
        Color fg = fromRgb(m_color);
        bg = Color::blend(fg, bg, 255);
        setPixel(rx, ry, toRgb(bg));
    }
    else
    {
        setPixel(rx, ry, m_color);
    }
}

void Display::Draw_FastHLine(int x, int y, int length)
{
    fillRect(m_x+x, m_y+y, length, 1, m_color);
}

void Display::Draw_FastVLine(int x, int y, int length)
{
    fillRect(m_x+x, m_y+y, 1, length, m_color);
}

// Used to do circles and roundrects
void Display::FillCircle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corner, int16_t delta)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while(x<y)  {
    if(f >= 0)  {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if(corner & 0x1)  {
      Draw_FastVLine(x0+x, y0-y, 2*y+1+delta);
      Draw_FastVLine(x0+y, y0-x, 2*x+1+delta);
    }
    if(corner & 0x2)  {
      Draw_FastVLine(x0-x, y0-y, 2*y+1+delta);
      Draw_FastVLine(x0-y, y0-x, 2*x+1+delta);
    }
  }
}

void Display::DrawCircle_Helper( int16_t x0, int16_t y0, int16_t r, uint8_t corner)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & 0x4) {
      drawPixel(x0 + x, y0 + y);
      drawPixel(x0 + y, y0 + x);
    }
    if (corner & 0x2) {
      drawPixel(x0 + x, y0 - y);
      drawPixel(x0 + y, y0 - x);
    }
    if (corner & 0x8) {
      drawPixel(x0 - y, y0 + x);
      drawPixel(x0 - x, y0 + y);
    }
    if (corner & 0x1) {
      drawPixel(x0 - y, y0 - x);
      drawPixel(x0 - x, y0 - y);
    }
  }
}


Display::Display(int width, int height, PixelFormat pixelFormat) :
    m_width(width), m_height(height),
    m_pixelFormat(pixelFormat)
{
    switch (pixelFormat)
    {
    case Format_ARGB8888:   m_bpp = 32; break;
    case Format_RGB888:     m_bpp = 24; break;
    case Format_RGB565:     m_bpp = 16; break;
    case Format_ARGB1555:   m_bpp = 16; break;
    case Format_ARGB4444:   m_bpp = 16; break;
    case Format_L8:         m_bpp = 8; break;
    case Format_AL44:       m_bpp = 8; break;
    case Format_AL88:       m_bpp = 16; break;
    case Format_A4:         m_bpp = 4; break;
    case Format_A8:         m_bpp = 8; break;
    case Format_L4:         m_bpp = 4; break;
    }

    //! @todo check alignment
//    m_bpl = ((m_bpp * m_width >> 3) + 3) & ~3;
    m_bpl = (m_bpp * m_width + 7) >> 3;
}

bool Display::hasAlphaChannel() const
{
    switch (m_pixelFormat)
    {
    case Format_ARGB8888:
    case Format_ARGB1555:
    case Format_ARGB4444:
    case Format_AL44:
    case Format_AL88:
    case Format_A4:
    case Format_A8:
        return true;
    default: return false;
    }
}
