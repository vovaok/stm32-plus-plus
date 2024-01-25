#ifndef _ETHTASK_H
#define _ETHTASK_H

#include "core/core.h"
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"
#include "ethernetif.h"
#include "netif/etharp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dhcp.h"
#include "lwip/igmp.h"
#include "lwip/ip_addr.h"
#include "core/timer.h"

class Ethernet
{
public:
    static Ethernet *instance();
    
    void setupNetworkInterface(const char *ipaddr, const char *netmask, const char *gateway);
    
    const ip_addr_t &address() const {return m_netif.ip_addr;}
    const ip_addr_t &netmask() const {return m_netif.netmask;}
    const ip_addr_t &gateway() const {return m_netif.gw;}
    ip_addr_t broadcast() const;
    
    static bool netcmp(const ip_addr_t &addr);
    static ip_addr_t ipFromString(const char *s);
    
private:
    Ethernet();
    static Ethernet *m_self;
    static bool s_llInitCompleted;
    
    struct netif m_netif;
    
    Timer m_tcpTimer, m_arpTimer, m_igmpTmr;
  
    void task();
};

#endif