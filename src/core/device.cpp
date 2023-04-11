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

ByteArray Device::read(int maxsize)
{
    ByteArray ba;
    if (maxsize > 0)
    {
        ba.resize(maxsize);
        int sz = readData(ba.data(), ba.size());
        ba.resize(sz);
    }
    return ba;
}

ByteArray Device::readAll()
{
    ByteArray ba;
    ba.resize(bytesAvailable());
    readData(ba.data(), ba.size());
    return ba;
}

int Device::write(const char *data, int size)
{
    return writeData(data, size);
}

int Device::write(const char *data)
{
    return writeData(data, strlen(data));
}

int Device::write(const ByteArray &data)
{
    return writeData(data.data(), data.size());
}

int Device::readLine(char *data, int maxsize)
{
    int sz = readLineData(data, maxsize - 1);
    data[sz] = '\0';
    return sz + 1;
}

ByteArray Device::readLine(int maxsize)
{
    ByteArray ba;
    if (maxsize)
    {
        ba.resize(maxsize);
        int sz = readLineData(ba.data(), maxsize);
        ba.resize(sz);
    }
    else
    {
        int offset = 0;
        while (!atEnd())
        {
            ba.resize(offset + 256);
            int sz = readLineData(ba.data() + offset, 256);
            if (ba[offset + sz - 1] == '\n')
            {
                ba.resize(offset + sz);
                break;
            }
            else
            {
                offset += 256;
            }
        }
    }
    return ba;
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