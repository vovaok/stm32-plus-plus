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

void Display::renderChar(char c, int &x, int &y)
{
	const FontInfo *fi = m_font.m_fi;
    if (!fi)
        return;
    
	const FontInfo::LetterInfo &li = fi->letterInfo(c);
	const uint8_t *src = fi->bitmap(c);
	int x0 = m_x + x + li.lb;
    int y0 = m_y + y - fi->a;
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
            Color bgcolor = Color::fromRgb565(pixel(x0+c, y0+r));
			Color col = Color::blend(m_color, bgcolor, v * 255 / mask);
			setPixel(x0+c, y0+r, col.rgb565());
		}
	}
	x += li.ha;
}

void Display::drawImage(int x, int y, const Image &img)
{
    if (!img.isNull())
    {
        copyRect(m_x+x, m_y+y, img.width(), img.height(), img.pixels());
    }
}

void Display::drawLine(int x0, int y0, int x1, int y1)
{
    uint16_t color = m_color.rgb565();
    if (x0 == x1)
    {
        if (x0 > x1)
            std::swap(x0, x1);
        Draw_FastVLine(x0, y0, y1 - y0 + 1);
    }
    else if (y0 == y1)
    {
        if (y0 > y1)
            std::swap(y0, y1);
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
                setPixel(m_x+y0, m_y+x0, color);
            else
                setPixel(m_x+x0, m_y+y0, color);
            err -= dy;
            if(err < 0)
            {
                y0 += ystep;
                err += dx;
            }
        }
    }
}

void Display::drawRect(int x, int y, int w, int h)
{
    Draw_FastHLine(x, y, w);
    Draw_FastHLine(x, y+h-1, w);
    Draw_FastVLine(x, y, h);
    Draw_FastVLine(x+w-1, y, h);
}

void Display::fillRect(int x, int y, int w, int h)
{
    fillRect(m_x+x, m_y+y, w, h, m_bgColor.rgb565());
}

void Display::drawFillRect(int x, int y, int w, int h)
{
    drawRect(x, y, w, h);
    fillRect(x+1, y+1, w-2, h-2);
}

void Display::drawFillCircle(int16_t x0, int16_t y0, int16_t r)
{
    Color temp = m_color;
    m_color = m_bgColor;
    Draw_FastVLine(x0, y0-r, 2*r+1);
    FillCircle_Helper(x0, y0, r, 3, 0);
    m_color = temp;
    drawCircle(x0, y0, r);
}

void Display::drawCircle(int16_t x0, int16_t y0, int16_t r)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  Draw_Pixel(x0  , y0+r);
  Draw_Pixel(x0  , y0-r);
  Draw_Pixel(x0+r, y0  );
  Draw_Pixel(x0-r, y0  );

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    Draw_Pixel(x0 + x, y0 + y);
    Draw_Pixel(x0 - x, y0 + y);
    Draw_Pixel(x0 + x, y0 - y);
    Draw_Pixel(x0 - x, y0 - y);
    Draw_Pixel(x0 + y, y0 + x);
    Draw_Pixel(x0 - y, y0 + x);
    Draw_Pixel(x0 + y, y0 - x);
    Draw_Pixel(x0 - y, y0 - x);
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

void Display::drawFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
    if (r > w/2)
        r = w/2;
    if (r > h/2)
        r = h/2;
    Color temp = m_color;
    m_color = m_bgColor;
    fillRect(x+r, y, w-2*r, h);
    FillCircle_Helper(x+w-r-1, y+r, r, 1, h-2*r-1);
    FillCircle_Helper(x+r, y+r, r, 2, h-2*r-1);
    m_color = temp;
    drawRoundRect(x, y, w, h, r);
}

void Display::Draw_Pixel(int x, int y)
{
    setPixel(m_x+x, m_y+y, m_color.rgb565());
}

void Display::Draw_FastHLine(int x, int y, int length)
{
    fillRect(m_x+x, m_y+y, length, 1, m_color.rgb565());
}

void Display::Draw_FastVLine(int x, int y, int length)
{
    fillRect(m_x+x, m_y+y, 1, length, m_color.rgb565());
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
      Draw_Pixel(x0 + x, y0 + y);
      Draw_Pixel(x0 + y, y0 + x);
    }
    if (corner & 0x2) {
      Draw_Pixel(x0 + x, y0 - y);
      Draw_Pixel(x0 + y, y0 - x);
    }
    if (corner & 0x8) {
      Draw_Pixel(x0 - y, y0 + x);
      Draw_Pixel(x0 - x, y0 + y);
    }
    if (corner & 0x1) {
      Draw_Pixel(x0 - y, y0 - x);
      Draw_Pixel(x0 - x, y0 - y);
    }
  }
}