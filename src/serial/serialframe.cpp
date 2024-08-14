#include "serialframe.h"
#include "core/application.h"

SerialFrame::SerialFrame(Device *device) :
    m_device(device)
{
    m_sequential = false;
    m_buffer.resize(64);
    cmd_acc = false;
    stmApp()->registerTaskEvent(EVENT(&SerialFrame::task));
}
//---------------------------------------------------------------------------

bool SerialFrame::open(OpenMode mode)
{
    if (m_device) 
        return m_device->open();
    return false;
}

void SerialFrame::task()
{
    if (m_ready)
        return;

//    char buferok[16];
//    int sz = m_device->read(buferok, 16);

//    for (int i=0; i<sz; i++)
    {
//        uint8_t byte = buferok[i];
        uint8_t byte;
        if (!m_device->read((char*)&byte, 1))
            return;

        switch (byte)
        {
          case uartESC:
            esc = true;
            break;

          case uartSOF:
            m_buffer.resize(0);
            cs = 0;
            esc = false;
            cmd_acc = true;
            break;

          case uartEOF:
            if (cmd_acc)
            {
                if (!cs && m_buffer.size())
                {
                    m_buffer.resize(m_buffer.size() - 1); // remove checksum
                    m_ready = true;
                    if (onReadyRead)
                        onReadyRead();
//                    dataReceived(m_buffer);
                }
                cmd_acc = false;
            }
            break;

          default:
            if (!cmd_acc)
                break;
            if (esc)
                byte ^= 0x20;
            esc = false;
            m_buffer.append(byte);
            cs += byte;
        }
    }
}
//----------------------------------------------------------

int SerialFrame::writeData(const char *data, int size)
{
    ByteArray out;
    uint8_t cs = 0;

    out.append(uartSOF);
    for (char i=0; i<size; i++)
    {
        char b = data[i];
        cs -= b;
        if (b == uartESC || b == uartSOF || b == uartEOF)
        {
            out.append(uartESC);
            b ^= 0x20;
        }
        out.append(b);
    }
    if (cs == uartESC || cs == uartSOF || cs == uartEOF)
    {
        out.append(uartESC);
        cs ^= 0x20;
    }
    out.append(cs);
    out.append(uartEOF);

    if (m_device->write(out.data(), out.size()) == out.size())
        return size;
    return 0;
}

int SerialFrame::bytesAvailable() const
{
    if (m_ready)
        return m_buffer.size();
    else
        return 0;
}

int SerialFrame::readData(char *data, int size)
{
    int sz = 0;
    if (m_ready)
    {
        sz = m_buffer.size();
        memcpy(data, m_buffer.data(), sz);
//        char *src = m_buffer.data();
//        while (sz--)
//            *data++ = *src++;

        m_ready = false;
    }
    task();
    return sz;
}
