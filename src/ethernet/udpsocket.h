#ifndef _UDPSOCKET_H
#define _UDPSOCKET_H

#include "lwip/udp.h"
#include "lwip/igmp.h"
#include "core/device.h"

class UdpSocket : public Device
{
public:
    UdpSocket();
    virtual ~UdpSocket();
    
    bool bind(uint16_t port);
    
    void connectToHost(ip_addr_t ipaddr, uint16_t port);
    bool open(OpenMode mode=ReadWrite);
    void close() override;
    
    bool joinMulticastGroup(ip_addr_t addr) const;
    
    ip_addr_t localAddress() const {return m_pcb->local_ip;}
    uint16_t localPort() const {return m_pcb->local_port;}
    
    bool hasPendingDatagrams() const;
    int writeDatagram(const char *data, int size, ip_addr_t addr, uint16_t port);
    int readDatagram(char *data, int maxsize, ip_addr_t *addr=nullptr, uint16_t *port=nullptr);
    
protected:
    int bytesAvailable() const override;
    int readData(char *data, int maxsize) override;
    int writeData(const char *data, int size) override;
    
private:
    struct udp_pcb *m_pcb;
    struct pbuf *m_p;
    ip_addr_t m_remoteAddr;
    uint16_t m_remotePort;
    
    void recv(struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
    static void s_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
};

#endif