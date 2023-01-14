#include "serialframe.h"

using namespace Serial;

SerialFrame::SerialFrame(Device *iface) :
    m_device(iface)
{
    cmd_acc = false;
    mFramesSent = 0;
    stmApp()->registerTaskEvent(EVENT(&SerialFrame::task));
}
//---------------------------------------------------------------------------

void SerialFrame::task()
{
    char buferok[16];
    int sz = m_device->read(buferok, 16);
    
    for (int i=0; i<sz; i++)
    {
        uint8_t byte = buferok[i];
        
        switch (byte)
        {
          case uartESC:
            esc = true;
            break;

          case uartSOF:
            m_buffer.clear();
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
                    dataReceived(m_buffer);
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

void SerialFrame::sendData(const ByteArray &data)
{
    ByteArray out;
    uint8_t cs = 0;

    out.append(uartSOF);

    for (char i=0; i<data.size(); i++)
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

    m_device->write(out.data(), out.size());
    mFramesSent++;
}
//----------------------------------------------------------

void SerialFrame::dataReceived(const ByteArray &ba)
{
    if (onDataReceived)
        onDataReceived(ba);
}
//----------------------------------------------------------

//void SerialFrame::attach(UartInterface *iface)
//{
//    mInterface = iface;
//    mInterface->setByteReadEvent(EVENT(&UartFrame::onByteRead));
//}