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

//! @todo use PixelFormat to calculate size!
Image::Image(const char *data, int width, int height) :
    Display(width, height)
{
    m_data = ByteArray::fromRawData(data, width * height * 2);
}

Image Image::fromData(const char *data, int size)
{
#pragma pack(push, 1)
    struct BmpHeader
    {
        uint16_t id;
        uint32_t fileSize;
        uint16_t _r1;
        uint16_t _r2;
        uint32_t offset;
    };
    typedef struct
    {
        uint32_t biSize;
        int32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t biXPelsPerMeter;
        int32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    } BITMAPINFOHEADER;
#pragma pack(pop)

    const BmpHeader *bmp = reinterpret_cast<const BmpHeader*>(data);
    if (bmp->id == 0x4d42) // "BM" is windows BMP file
    {
        const BITMAPINFOHEADER *info = reinterpret_cast<const BITMAPINFOHEADER *>(data + 14);
        Image img;
        img.m_width = info->biWidth;
        img.m_height = info->biHeight > 0? info->biHeight: -info->biHeight;
        img.m_data = ByteArray::fromRawData(data + bmp->offset, info->biSizeImage);
        // img.bpp = biBitCount >> 3;
        //! @todo use BPP
        //! @todo deal with bitmap rows 32-bit alignment
        return img;
    }

    return Image();
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
