#include "udpsocket.h"

UdpSocket::UdpSocket() :
    m_p(nullptr),
    m_remoteAddr({0}),
    m_remotePort(0)
{
    m_pcb = udp_new();

    if (!m_pcb)
        THROW(Exception::BadSoBad);
}

UdpSocket::~UdpSocket()
{
    udp_remove(m_pcb);
}

bool UdpSocket::bind(uint16_t port)
{
    err_t err = udp_bind(m_pcb, IP_ADDR_ANY, port);
    if (err == ERR_OK)
        udp_recv(m_pcb, &UdpSocket::s_recv, this);
//    else
//        THROW(Exception::BadSoBad);
    
    return (err == ERR_OK);
}

void UdpSocket::recv(struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    m_remoteAddr = *addr;
    m_remotePort = port;
  
//    udp_connect(upcb, addr, UDP_CLIENT_PORT);
//    if (!isOpen())
//        open(); // tell that device is opened
    
    // drop the last unread packet
    if (m_p)
        pbuf_free(m_p);
    
    m_p = p;
    
    if (isOpen() && onReadyRead)
        onReadyRead();
}

static int udpPacketsReceived = 0;

void UdpSocket::s_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    udpPacketsReceived++;
    if (arg)
        return reinterpret_cast<UdpSocket *>(arg)->recv(upcb, p, addr, port);
}

//bool UdpSocket::open(OpenMode mode)
void UdpSocket::connectToHost(ip_addr_t ipaddr, uint16_t port)
{
    if (ipaddr.addr)
    {
        m_remoteAddr = ipaddr;
        m_remotePort = port;
        if (ERR_OK == udp_connect(m_pcb, &m_remoteAddr, port))
            Device::open();
    }
}

bool UdpSocket::open(Device::OpenMode mode)
{
    return false; // use connectToHost instead
}

void UdpSocket::close()
{
    udp_disconnect(m_pcb);
    Device::close();
}

bool UdpSocket::joinMulticastGroup(ip_addr_t addr) const
{
    err_t err;
    
    {
    //    TcpipCoreLock lock;
        err = igmp_joingroup(IP_ADDR_ANY, &addr);
    }

    return (err == ERR_OK);
}

int UdpSocket::bytesAvailable() const
{
    if (m_p)
        return m_p->tot_len;
    return 0;
//    int l = 0;
//    for (pbuf *q = m_p; q != NULL; q = q->next)
//        l = l + q->len;
//    return l;
}

int UdpSocket::readData(char *data, int maxsize)
{
    pbuf *q = m_p;
    int l = 0;
    if (q)
    {
        for (q = m_p; q != NULL; q = q->next)
        {
            int over = (l + q->len) - maxsize;
            if (over < 0)
                over = 0;
            memcpy(data + l, q->payload, q->len - over);
            l = l + q->len;
            if (over)
                break;
        }    
    }
    if (m_p)
    {
        pbuf_free(m_p);
        m_p = nullptr;
    }
    return l;
}

int UdpSocket::writeData(const char *data, int size)
{
    pbuf *q, *p;
    int l = 0;
    
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

    /* copy received frame to pbuf chain */
    if (p != NULL)
    {
        for (q = p; q != NULL; q = q->next)
        {
            memcpy((u8_t*)q->payload, (const u8_t*)&data[l], q->len);
            l = l + q->len;
        }
        
        udp_send(m_pcb, p);
    }
    else
    {
        size = 0;
    }
    
    pbuf_free(p);
    return size;
}

bool UdpSocket::hasPendingDatagrams() const
{
    return m_p;
}

int UdpSocket::writeDatagram(const char *data, int size, ip_addr_t addr, uint16_t port)
{
    err_t err = ERR_MEM;
    pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    if (p)
    {    
        err = pbuf_take(p, data, size);
        if (err == ERR_OK)
            err = udp_sendto(m_pcb, p, &addr, port);
    }
    
    pbuf_free(p);
    if (err == ERR_OK)
        return size;
    return 0;
}

int UdpSocket::readDatagram(char *data, int maxsize, ip_addr_t *addr, uint16_t *port)
{
    if (m_p)
    {
        if (addr)
            *addr = m_remoteAddr;
        if (port)
            *port = m_remotePort;
        return readData(data, maxsize);
    }
    
    return 0;
}