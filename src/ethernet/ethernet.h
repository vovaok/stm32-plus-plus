#ifndef _ETHTASK_H
#define _ETHTASK_H

#include "core/core.h"
#include "core/timer.h"
#include "stm32f4x7_eth.h"
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
#include "gpio.h"

class RMII
{
public:
    int phyAddress = 0x01;
    Gpio::Config pinMDC     = Gpio::ETH_MDC_PC1;
    Gpio::Config pinMDIO    = Gpio::ETH_MDIO_PA2;
    Gpio::Config pinREF_CLK = Gpio::ETH_RMII_REF_CLK_PA1;
    Gpio::Config pinCRS_DV  = Gpio::ETH_RMII_CRS_DV_PA7;
    Gpio::Config pinRXD0    = Gpio::ETH_RMII_RXD0_PC4;
    Gpio::Config pinRXD1    = Gpio::ETH_RMII_RXD1_PC5;
    Gpio::Config pinTX_EN   = Gpio::ETH_RMII_TX_EN_PB11;
    Gpio::Config pinTXD0    = Gpio::ETH_RMII_TXD0_PB12;
    Gpio::Config pinTXD1    = Gpio::ETH_RMII_TXD1_PB13;
    Gpio::PinName pinReset  = Gpio::noPin;
};

class Ethernet
{
public:    
    static Ethernet *instance();
    static Ethernet *instance(const RMII &rmii);
    
    bool isValid() const {return s_llInitCompleted;}
    
    void setupNetworkInterface(const char *ipaddr, const char *netmask, const char *gateway);
    
    const ip_addr_t &address() const {return m_netif.ip_addr;}
    const ip_addr_t &netmask() const {return m_netif.netmask;}
    const ip_addr_t &gateway() const {return m_netif.gw;}
    ip_addr_t broadcast() const;
    
    static bool netcmp(const ip_addr_t &addr);
    static ip_addr_t ipFromString(const char *s);
    
private:
    Ethernet(const RMII &rmii);
    static Ethernet *m_self;
    static bool s_llInitCompleted;
    
    struct netif m_netif;
    
    Timer m_tcpTimer, m_arpTimer, m_igmpTmr;
  
    void task();
    
    bool ethConfig(uint16_t phyAddress);
};

#endif