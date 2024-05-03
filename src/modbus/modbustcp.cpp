#include "modbustcp.h"

using namespace Modbus;

ModbusTcp::ModbusTcp(uint16_t port) :
    TcpServer()
{
    m_buffer.resize(255);
    m_buffer.resize(0);
    m_txBuffer.resize(63);
    m_txBuffer.resize(0);
    listen(port);
}

void ModbusTcp::incomingConnection(SocketDescriptor_t pcb)
{
    if (m_socket)
    {
        onSocketDisconnect();
    }
    m_socket = new TcpSocket();
    m_socket->setSocketDescriptor(pcb);
    m_socket->onDisconnect = EVENT(&ModbusTcp::onSocketDisconnect);
    m_socket->onReadyRead = EVENT(&ModbusTcp::onSocketRead);
}

void ModbusTcp::onSocketDisconnect()
{
    m_socket->close();
    delete m_socket;
    m_socket = nullptr;
}

void ModbusTcp::onSocketRead()
{
    if (m_socket)
        m_buffer.append(m_socket->readAll());

    while (m_buffer.size() > sizeof(Header))
    {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(m_buffer.data());
        Header hdr;
        hdr.transactionId = readWord(data);
        hdr.protocolId = readWord(data);
        hdr.length = readWord(data);
        hdr.unitId = *data++;

        if (hdr.protocolId != 0)
        {
            m_buffer.resize(0); // EROORE!
            break;
        }

        int len = hdr.length + 6;
        if (m_buffer.size() >= len) // length including the first three fields
        {
            m_transactionId = hdr.transactionId;
            ADU adu;
            adu.addr = hdr.unitId;
            adu.func = *data++;
            adu.data = reinterpret_cast<const char *>(data);
            adu.size = hdr.length - 2; // excluding unitId and functionCode

            receiveADU(adu);

            m_buffer.remove(0, len);
        }
        else
        {
            break;
        }
    }
}

void ModbusTcp::writeADU(const ADU &adu)
{
    int len = adu.size + sizeof(Header) + 1;
    m_txBuffer.resize(len); // including functionCode

    uint8_t *data = reinterpret_cast<uint8_t *>(m_txBuffer.data());
    writeWord(data, m_transactionId);
    writeWord(data, 0); // protocolId
    writeWord(data, adu.size + 2); // length
    *data++ = adu.addr; // unitId
    *data++ = adu.func;
    int sz = adu.size;
    const char *src = adu.data;
    while (sz--)
        *data++ = *src++;

    if (m_socket)
        m_socket->write(m_txBuffer);
}
