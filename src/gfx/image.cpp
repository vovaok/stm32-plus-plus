#include "image.h"

Image::Image() :
    FrameBuffer()
{

}

Image::Image(int width, int height, PixelFormat pixelFormat) :
    FrameBuffer(nullptr, width, height, pixelFormat)
{
    m_buffer.resize(sizeInBytes());
    m_data = reinterpret_cast<uint8_t *>(m_buffer.data());
}

Image::Image(const char *data, int width, int height, PixelFormat pixelFormat) :
    FrameBuffer((uint8_t *)data, width, height, pixelFormat)
{
    m_buffer = ByteArray::fromRawData(data, sizeInBytes());
}

Image::Image(const Image &other) :
    FrameBuffer(other)
{
    m_buffer = other.m_buffer;
    m_data = reinterpret_cast<uint8_t *>(m_buffer.data());
}

Image::Image(Image &&other) :
    FrameBuffer(std::move(other))
{
    m_buffer = std::move(other.m_buffer);
    m_data = reinterpret_cast<uint8_t *>(m_buffer.data());
}

Image &Image::operator =(const Image &other)
{
    FrameBuffer::operator=(other);
    m_buffer = other.m_buffer;
    m_data = reinterpret_cast<uint8_t *>(m_buffer.data());
    return *this;
}

Image &Image::operator =(Image &&other)
{
    FrameBuffer::operator=(other);
    m_buffer = std::move(other.m_buffer);
    m_data = reinterpret_cast<uint8_t *>(m_buffer.data());
    return *this;
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
    if (bmp->id == 0x4d42) // "BM" is Windows BMP file
    {
        const BITMAPINFOHEADER *info = reinterpret_cast<const BITMAPINFOHEADER *>(data + 14);
        int w = info->biWidth;
        int h = info->biHeight > 0? info->biHeight: -info->biHeight;

        //! @todo use BPP!!
        // img.bpp = biBitCount >> 3;

        return Image(data + bmp->offset, w, h, Format_RGB565);
    }

    return Image();
}

void Image::fill(Color color)
{
    FrameBuffer::fill(toRgb(color));
}

void Image::setPixelColor(int x, int y, Color color)
{
    setPixel(x, y, toRgb(color));
}

Color Image::pixelColor(int x, int y) const
{
    return fromRgb(pixel(x, y));
}

bool Image::valid(int x, int y)
{
    return (x >= 0 && x < width() && y >= 0 && y < height());
}
