#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include "lwip/tcp.h"
//#include "core/timer.h"
#include <vector>
#include "tcpsocket.h"

class TcpServer
{
public:
    TcpServer();
    
    void listen(uint16_t port);
    void close();
    
    Closure<void(TcpSocket *)> onNewConnection;
    
protected:
    virtual void incomingConnection(struct tcp_pcb *pcb);
  
private:
    struct tcp_pcb *m_pcb;
    bool m_listening;
    std::vector<TcpSocket *> m_connections;
    
//    void task();
    err_t accept(struct tcp_pcb *newpcb, err_t err);
    
    static err_t s_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
};

#endif