#include "ethernet.h"

extern __IO uint32_t LocalTime;
Ethernet *Ethernet::m_self = nullptr;
bool Ethernet::s_llInitCompleted = false;

extern "C" void tcp_tmr();

Ethernet *Ethernet::instance()
{
    if (!m_self)
        m_self = new Ethernet(RMII());
    return m_self;
}

Ethernet *Ethernet::instance(const RMII &rmii)
{
    if (!m_self)
        m_self = new Ethernet(rmii);
    return m_self;
}

Ethernet::Ethernet(const RMII &rmii)
{    
    if (!s_llInitCompleted)
    {
        rcc().setPeriphEnabled(SYSCFG);
        SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;
        
        Gpio::config(rmii.pinMDC);
        Gpio::config(rmii.pinMDIO);
        Gpio::config(rmii.pinREF_CLK);
        Gpio::config(rmii.pinCRS_DV);
        Gpio::config(rmii.pinRXD0);
        Gpio::config(rmii.pinRXD1);
        Gpio::config(rmii.pinTX_EN);
        Gpio::config(rmii.pinTXD0);
        Gpio::config(rmii.pinTXD1);
        Gpio *pinReset = new Gpio(rmii.pinReset, Gpio::Output);
        pinReset->write(0);
        for (int w=20000; --w;);
        pinReset->write(1);
        for (int w=20000; --w;);
        
        bool result = ethConfig(rmii.phyAddress);
        if (!result)
            return;

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
        
        s_llInitCompleted = true;
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

bool Ethernet::ethConfig(uint16_t phyAddress)
{
    ETH_InitTypeDef ETH_InitStructure;

    /* Enable ETHERNET clock  */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                        RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                        
    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* Software reset */
    ETH_SoftwareReset();

    uint32_t timeout = 1000;
    /* Wait for software reset */
    while (ETH_GetSoftwareResetStatus() == SET)
    {
        --timeout;
        if (!timeout)
        {
            return false;
        }
    }

    /* ETHERNET Configuration --------------------------------------------------*/
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);

    /* Fill ETH_InitStructure parametrs */
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
    //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
    //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    #ifdef CHECKSUM_BY_HARDWARE
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
    #endif

    /*------------------------   DMA   -----------------------------------*/  

    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     

    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    /* Configure Ethernet */
    return ETH_Init(&ETH_InitStructure, phyAddress);
}
