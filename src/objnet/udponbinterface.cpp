#include "udponbinterface.h"

using namespace Objnet;

UdpOnbInterface::UdpOnbInterface() :
    GenericOnbInterface(m_socket = new UdpSocket),
    m_broadcastCnt(0)
{
    mBusType = BusEthernet;
    prefix = reinterpret_cast<const uint32_t*>("ONB1");
    
    m_socket->bind(51967);
    
    m_advertiseTimer = new Timer;
    m_advertiseTimer->onTimeout = EVENT(&UdpOnbInterface::advertise);
    m_advertiseTimer->start(100);
}

UdpOnbInterface::~UdpOnbInterface()
{
    m_socket->close();
    delete m_socket;
}

void UdpOnbInterface::advertise()
{
    if (!m_socket->isOpen())
    {
        if (m_socket->hasPendingDatagrams())
        {
            ip_addr_t addr;
            uint16_t port;
            char buf[4];
            int size = m_socket->readDatagram(buf, 4, &addr, &port);
            if (size > 4 && !strncmp(buf, "ONB1", 4))
            {
                m_socket->connectToHost(addr, port);
                m_advertiseTimer->stop();
            }
        }
        else if (++m_broadcastCnt >= 10)
        {
            m_broadcastCnt = 0;
            ip_addr_t broadcast = Ethernet::instance()->broadcast();
            m_socket->writeDatagram("ONB1", 4, broadcast, 51967);
        }
    }
}

void UdpOnbInterface::reconnect()
{
    m_socket->close();
    if (!m_advertiseTimer->isRunning())
        m_advertiseTimer->start();
}