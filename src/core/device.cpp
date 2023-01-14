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