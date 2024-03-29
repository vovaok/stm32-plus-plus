#include "deviceframed.h"

DeviceFramed::DeviceFramed(Device *dev) :
    m_device(dev)
{
    m_rxBuffer.resize(255);
}

int DeviceFramed::bytesAvailable() const
{
    //! @todo NEDOPILENO!

}

int DeviceFramed::writeData(const char *data, int size)
{
    uint8_t cs = 0;
    if (m_device->write("{", 1) <= 0)
        return 0;

    int sz = size + 1;
    while (sz--)
    {
        char b = *data++;

        if (sz)
            cs -= b;
        else
            b = cs;

        switch (b)
        {
        case '{':
        case '}':
        case '\\':
            if (m_device->write("\\", 1) <= 0)
                return 0;
            b ^= 0x20;
        }
        if (m_device->write(&b, 1) <= 0)
            return 0;
    }
    if (m_device->write("}", 1) <= 0)
        return 0;
    return size;
}

int DeviceFramed::readData(char *data, int size)
{
//    m_device->read()
    //! @todo NEDOPILENO!

}