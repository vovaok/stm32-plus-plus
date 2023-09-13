#include "image.h"

Image::Image() :
    Display(0, 0)
{
    
}

Image::Image(int width, int height) :
    Display(width, height)
{
    m_data.resize(width * height * 2);
}

void Image::setPixel(int x, int y, uint16_t color)
{
    if (valid(x, y))
        pixels()[m_width * y + x] = color;
}

uint16_t Image::pixel(int x, int y)
{
    if (valid(x, y))
        return pixels()[m_width * y + x];
    return 0;
}

void Image::setPixelColor(int x, int y, Color color)
{
    setPixel(x, y, color.rgb565());
}

Color Image::pixelColor(int x, int y)
{
    return Color::fromRgb565(pixel(x, y));
}

bool Image::valid(int x, int y)
{
    return (x >= 0 && x < m_width && y >= 0 && y < m_height);
}

void Image::fill(uint16_t color)
{
    if (isNull())
        return;
    uint16_t *dst = pixels();
    int cnt = m_width * m_height;
    while (cnt--)
    {
        *dst++ = color;
    }
}

void Image::fillRect(int x, int y, int width, int height, uint16_t color)
{
    if (width <= 0 || height <= 0)
        return;
    if (x >= m_width || y >= m_height)
        return;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x + width > m_width)
        width = m_width - x;
    if (y + height > m_height)
        height = m_height - y;
        
    int ey = y + height;
    for (int i=y; i<ey; i++)
    {
        uint16_t *dst = scanLine(i) + x;
        int cnt = width;
        while (cnt--)
            *dst++ = color;
    }
}

void Image::copyRect(int x, int y, int width, int height, const uint16_t *buffer)
{
    if (width <= 0 || height <= 0)
        return;
    if (x >= m_width || y >= m_height)
        return;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x + width > m_width)
        width = m_width - x;
    if (y + height > m_height)
        height = m_height - y;
        
    int ey = y + height;
    for (int i=y; i<ey; i++)
    {
        uint16_t *dst = scanLine(i) + x;
        int cnt = width;
        while (cnt--)
            *dst++ = *buffer++;
    }
}
    