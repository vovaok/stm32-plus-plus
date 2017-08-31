#include "serialframe.h"

using namespace Serial;

SerialFrame::SerialFrame(SerialInterface *iface) :
    mInterface(iface)
{
    cmd_acc = false;
    mFramesSent = 0;
    //mInterface->setByteReadEvent(EVENT(&UartFrame::onByteRead));
    mInterface->onReadyRead = EVENT(&SerialFrame::onReadyRead);
}
//---------------------------------------------------------------------------

void SerialFrame::onReadyRead()
{
    ByteArray ba;
    int sz = mInterface->read(ba);
    
    for (int i=0; i<sz; i++)
    {
        unsigned char byte = ba[i];
        
        switch (byte)
        {
          case uartESC:
            esc = true;
            break;

          case uartSOF:
            buffer.clear();
            cs = 0;
            esc = false;
            cmd_acc = true;
            break;

          case uartEOF:
            if (cmd_acc)
            {
                if (!cs && buffer.size())
                {
                    buffer.resize(buffer.size() - 1); // remove checksum
                    dataReceived(buffer);
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
            buffer.append(byte);
            cs += byte;
        }
    }
}
//----------------------------------------------------------

void SerialFrame::sendData(const ByteArray &data)
{
    ByteArray out;
    char cs = 0;

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

    mInterface->write(out);
    mFramesSent++;
}
//----------------------------------------------------------

void SerialFrame::dataReceived(const ByteArray &ba)
{
    if (mDataReceived)
        mDataReceived(ba);
}
//----------------------------------------------------------

//void SerialFrame::attach(UartInterface *iface)
//{
//    mInterface = iface;
//    mInterface->setByteReadEvent(EVENT(&UartFrame::onByteRead));
//}