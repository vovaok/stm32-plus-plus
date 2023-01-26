#include "device.h"

Device::Device() :
    m_sequential(true),
    m_halfDuplex(false),
    m_openMode(NotOpen)
{
}

bool Device::open(OpenMode mode)
{
    m_openMode = mode;
    return true;
}

void Device::close()
{
    m_openMode = NotOpen;
}

int Device::read(char *data, int maxsize)
{
    return readData(data, maxsize);
}

int Device::write(const char *data, int size)
{
    return writeData(data, size);
}

int Device::readLine(char *data, int maxsize)
{
    int sz = readLineData(data, maxsize - 1);
    data[sz] = '\0';
    return sz + 1;
}

int Device::readLineData(char *data, int size)
{
    char b;
    int sz = 0;
    do
    {
        if (!readData(&b, 1))
            break;
        *data++ = b;
        sz++;
        if (sz > size)
            break;
    } while (b && b != '\n');
    return sz;
}