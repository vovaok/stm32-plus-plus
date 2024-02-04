#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H

#include "lwip/tcp.h"
#include "core/device.h"

typedef struct tcp_pcb * SocketDescriptor_t;

class TcpSocket : public Device
{
public:
    TcpSocket();
    TcpSocket(SocketDescriptor_t pcb);
    virtual ~TcpSocket();
    
    void setSocketDescriptor(SocketDescriptor_t pcb);
    
    bool isSequential() const {return true;}
    int bytesAvailable() const override;
    
    bool canReadLine() const;
    
    NotifyEvent onConnect;
    NotifyEvent onDisconnect;
    
    void connectToHost(ip_addr_t ipaddr, uint16_t port);
    
    void close();
    
protected:
    int readData(char *data, int maxsize) override;
    int writeData(const char *data, int size) override;
//    int readLineData(char *data, int size); // use default implementation
      
private:
    friend class TcpServer;
    struct tcp_pcb *m_pcb;
    struct pbuf *m_p;
    uint16_t m_pOffset;
    ByteArray m_sendBuf;
    
    err_t recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    void error(err_t err);
    err_t poll(struct tcp_pcb *tpcb);
    err_t sent(struct tcp_pcb *tpcb, uint16_t len);
    err_t connected(struct tcp_pcb *tpcb, err_t err);
    
    static err_t s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static void s_error(void *arg, err_t err);
    static err_t s_poll(void *arg, struct tcp_pcb *tpcb);
    static err_t s_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len);
    static err_t s_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
};

#endif