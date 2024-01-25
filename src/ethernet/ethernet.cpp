#include "ethernet.h"

extern __IO uint32_t LocalTime;
Ethernet *Ethernet::m_self = nullptr;
bool Ethernet::s_llInitCompleted = false;

extern "C" void tcp_tmr();

Ethernet *Ethernet::instance()
{
    if (!m_self)
        m_self = new Ethernet();
    return m_self;
}

Ethernet::Ethernet()
{    
    if (!s_llInitCompleted)
    {
        /* configure ethernet */ 
        if (ETH_BSP_Config() == ETH_ERROR)
            THROW(Exception::BadSoBad);

        /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
        mem_init();

        /* Initializes the memory pools defined by MEMP_NUM_x.*/
        memp_init();  
        
        lwip_init();
        
        m_tcpTimer.callback = tcp_tmr;
        m_tcpTimer.start(250);

        m_arpTimer.callback = etharp_tmr;
        m_arpTimer.start(5000);
        
        m_igmpTmr.callback = igmp_tmr;
        m_igmpTmr.start(IGMP_TMR_INTERVAL);
        
        stmApp()->registerTaskEvent(EVENT(&Ethernet::task));
    }
}

void Ethernet::setupNetworkInterface(const char *ipaddr, const char *netmask, const char *gateway)
{
    ip_addr_t addr, mask, gw;
    addr = ipFromString(ipaddr);
    mask = ipFromString(netmask);
    gw = ipFromString(gateway);
    
    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
        struct ip_addr *netmask, struct ip_addr *gw,
        void *state, err_t (* init)(struct netif *netif),
        err_t (* input)(struct pbuf *p, struct netif *netif))

    Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    netif_add(&m_netif, &addr, &mask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /*  Registers the default network interface.*/
    netif_set_default(&m_netif);

    /*  When the netif is fully configured this function must be called.*/
    netif_set_up(&m_netif);
    
    m_netif.flags |= NETIF_FLAG_IGMP;
//    igmp_start(&m_netif);
}

bool Ethernet::netcmp(const ip_addr_t &addr)
{
    return ip_addr_netcmp(&m_self->m_netif.ip_addr, &addr, &m_self->m_netif.netmask);
}

ip_addr_t Ethernet::ipFromString(const char *s)
{
    union
    {
        ip_addr_t ipaddr;
        uint8_t ip[4];
    };
    ByteArray ba(s);
    int i1 = 0, i2 = 0;
    for (int i=0; i<4; i++)
    {
        i2 = ba.indexOf('.', i1);
        if (i2 < 0)
            i2 = ba.size();
        ip[i] = ba.mid(i1, i2 - i1).toInt();
        i1 = i2 + 1;
    }
    return ipaddr;
}

ip_addr_t Ethernet::broadcast() const
{
    ip_addr_t result = m_netif.ip_addr;
    result.addr |= ~m_netif.netmask.addr; // ala x.x.x.255
    return result;
}

void Ethernet::task()
{
    /* check if any packet received */
    while (ETH_CheckFrameReceived())
    { 
        /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
        ethernetif_input(&m_netif);
    }
}
