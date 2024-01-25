#include "tcpserver.h"

TcpServer::TcpServer() :
    m_listening(false)
{
//    tcp_init(); // should be already done
    m_pcb = tcp_new();
}

//void TcpServer::task()
//{
//    if (m_listening)
//    {
//        
//    }
//}

void TcpServer::listen(uint16_t port)
{
    tcp_arg(m_pcb, this);
    tcp_bind(m_pcb, IP_ADDR_ANY, port);
    m_pcb = tcp_listen(m_pcb);
    tcp_accept(m_pcb, &TcpServer::s_accept);
    m_listening = true;
}

void TcpServer::close()
{
    tcp_close(m_pcb);
    m_pcb = 0L;
    m_listening = false;
    for (int i=0; i<m_connections.size(); i++)
    {
        m_connections[i]->close();
        delete m_connections[i];
    }
    m_connections.clear();
}

void TcpServer::incomingConnection(struct tcp_pcb *pcb)
{
    TcpSocket *socket = new TcpSocket(pcb);
    m_connections.push_back(socket);
    if (onNewConnection)
        onNewConnection(socket);
}

err_t TcpServer::accept(struct tcp_pcb *newpcb, err_t err)
{
    // tcp_setprio(newpcb, TCP_PRIO_MIN);
    incomingConnection(newpcb);
    tcp_accepted(m_pcb);
    return ERR_OK;
}

err_t TcpServer::s_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    if (arg)
        return reinterpret_cast<TcpServer*>(arg)->accept(newpcb, err);
    return ERR_ARG;
}