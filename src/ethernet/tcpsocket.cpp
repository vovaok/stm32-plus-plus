#include "tcpsocket.h"

TcpSocket::TcpSocket() :
    m_pcb(0L),
    m_p(0L)
{
    
}

TcpSocket::TcpSocket(struct tcp_pcb *pcb) :
    m_pcb(pcb),
    m_p(0L)
{
    tcp_arg(m_pcb, this);
    tcp_recv(m_pcb, &TcpSocket::s_recv);
    tcp_err(m_pcb, &TcpSocket::s_error);
    tcp_poll(m_pcb, &TcpSocket::s_poll, 1);
    tcp_sent(m_pcb, &TcpSocket::s_sent);
    Device::open(Device::ReadWrite);
}

TcpSocket::~TcpSocket()
{
    close();
}   

void TcpSocket::connectToHost(ip_addr_t ipaddr, uint16_t port)
{
    if (m_pcb)
        return;
    
    m_pcb = tcp_new();
    tcp_arg(m_pcb, this);
    tcp_recv(m_pcb, &TcpSocket::s_recv);
    tcp_err(m_pcb, &TcpSocket::s_error);
    tcp_poll(m_pcb, &TcpSocket::s_poll, 1);
    tcp_sent(m_pcb, &TcpSocket::s_sent);
    tcp_connect(m_pcb, &ipaddr, port, &TcpSocket::s_connected);
}

void TcpSocket::close()
{
    if (m_pcb)
    {
        Device::close();
        
        // this is probably not necessary:
    //    tcp_arg(m_pcb, 0L);
    //    tcp_recv(m_pcb, 0L);
    //    tcp_err(m_pcb, 0L);
    //    tcp_poll(m_pcb, 0L, 0);
    //    tcp_sent(m_pcb, 0L);
        
        if (onDisconnect && isOpen())
            onDisconnect();
        
        tcp_close(m_pcb); // m_pcb id deallocated by lwIP 
        m_pcb = 0L;
    }
}

int TcpSocket::bytesAvailable() const
{
    if (m_pcb && m_p)
        return m_p->tot_len;
    
//    int len = 0;
//    for (pbuf *q = m_p; q != 0L; q = q->next)
//        len = len + q->len;
    return 0;
}

bool TcpSocket::canReadLine() const
{
    if (!m_pcb)
        return false;
    for (pbuf *q = m_p; q != 0L; q = q->next)
    {
        for (int i=0; i<q->len; i++)
            if (((char*)q->payload)[i] == '\n')
                return true;
    }
    return false;
}

int TcpSocket::readData(char *data, int maxsize)
{
    if (!m_pcb)
        return 0;
    
    int len = 0;
    if (m_p)
    {
        len = pbuf_copy_partial(m_p, data, maxsize, m_pOffset);
        m_pOffset += len;
        tcp_recved(m_pcb, len);
        if (m_pOffset >= m_p->tot_len)
        {
            pbuf_free(m_p);
            m_p = 0L;
            m_pOffset = 0;
        }
    }
    
//    pbuf *q = m_p;
//    int len = 0;
//    if (q)
//    {
//        for (q = m_p; q != NULL; q = q->next)
//        {
//            int over = maxsize - (len + q->len);
//            if (over > 0)
//                over = 0;
//            memcpy((u8_t*)&data[len], (u8_t*)q->payload, q->len - over);
//            len = len + q->len;
//            if (over)
//                break;
//        }    
//    }
    
    return len;
}

int TcpSocket::writeData(const char *data, int size)
{
    if (!m_pcb)
        return 0;
    
    m_sendBuf.append(data, size);
    size = m_sendBuf.size();
    
    int maxsize = tcp_sndbuf(m_pcb);
//    int length = 0;
    if (size > maxsize)
    {
//        length = size - maxsize;
        size = maxsize;
    }
     
    err_t wr_err = tcp_write(m_pcb, m_sendBuf.data(), size, 1);
    
    if (wr_err == ERR_MEM)
    {
        size = 0; // return 0; // no data written actually
    }
    else
    {
        m_sendBuf.remove(0, size);
    }
    
    return size;
}

//int TcpSocket::readLineData(char *data, int size)
//{
//    
//}
    
err_t TcpSocket::recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)
    {
        close();
        return ERR_OK;
    }
    else if (err != ERR_OK)
    {
        pbuf_free(p);
        return err;
    }
    else
    {
        if (!m_p)
        {
            m_p = p;
            m_pOffset = 0;
        }
        else
        {
            pbuf_chain(m_p, p); // append new data to the end
        }
        if (onReadyRead)
            onReadyRead();
        return ERR_OK;
    }
    
//    return ERR_MEM;
}

void TcpSocket::error(err_t err)
{
  
}

//static bool flag;
err_t TcpSocket::poll(struct tcp_pcb *tpcb)
{
    
    return ERR_OK;
}

err_t TcpSocket::sent(struct tcp_pcb *tpcb, uint16_t len)
{
    if (m_sendBuf.size())
    {
        writeData(nullptr, 0);
//        ByteArray ba = m_sendBuf;
//        m_sendBuf.clear();
//        writeData(ba.data(), ba.size());
    }
    return ERR_OK;
}

err_t TcpSocket::connected(struct tcp_pcb *tpcb, err_t err)
{
    Device::open(Device::ReadWrite);
    if (onConnect)
        onConnect();
    return ERR_OK;
}

err_t TcpSocket::s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (arg)
        return reinterpret_cast<TcpSocket *>(arg)->recv(tpcb, p, err);
    return ERR_ARG;
}

void TcpSocket::s_error(void *arg, err_t err)
{
    if (arg)
        reinterpret_cast<TcpSocket *>(arg)->error(err);
}

err_t TcpSocket::s_poll(void *arg, struct tcp_pcb *tpcb)
{
    if (arg)
        return reinterpret_cast<TcpSocket *>(arg)->poll(tpcb);
    return ERR_ARG;
}

err_t TcpSocket::s_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
{
    if (arg)
        return reinterpret_cast<TcpSocket *>(arg)->sent(tpcb, len);
    return ERR_ARG;
}

err_t TcpSocket::s_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    if (arg)
        return reinterpret_cast<TcpSocket *>(arg)->connected(tpcb, err);
    return ERR_ARG;
}