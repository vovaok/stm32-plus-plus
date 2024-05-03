#pragma once

#include "ethernet/tcpserver.h"
#include "ethernet/tcpsocket.h"
#include "modbustransport.h"

class ModbusTcp : public TcpServer, public ModbusTransport
{
public:
    ModbusTcp(uint16_t port = 502);
    
protected:
    void incomingConnection(SocketDescriptor_t pcb);
    
    virtual void writeADU(const Modbus::ADU &adu) override;
    
private:
    TcpSocket *m_socket = nullptr;
    ByteArray m_buffer;
    ByteArray m_txBuffer;
    uint16_t m_transactionId;
    
    void onSocketDisconnect();
    void onSocketRead();
    
    // BIG endian!!!
#pragma pack(push,1)
    struct Header
    {
        uint16_t transactionId;
        uint16_t protocolId;
        uint16_t length;
        uint8_t unitId;
    };
#pragma pack(pop)
};
