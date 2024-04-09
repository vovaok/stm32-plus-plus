#include "ethernet.h"

Ethernet::MacAddress_t Ethernet::defaultMacAddress = {0xF8, 0x5C, 0x4D, 0x33, 0x10, 0x00};
int Ethernet::rxBufCount = 5;
int Ethernet::txBufCount = 5;
int Ethernet::rxBufSize = ETH_MAX_PACKET_SIZE;
int Ethernet::txBufSize = ETH_MAX_PACKET_SIZE;

/* Global pointers to track current transmit and receive descriptors */
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;

/* Global pointer for last received frame infos */
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;

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
        if (!m_self)
            m_self = this;
        
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
        
        m_DMARxDscrTab = new ETH_DMADESCTypeDef[rxBufCount];
        m_DMATxDscrTab = new ETH_DMADESCTypeDef[txBufCount];
        m_rxBuff = new uint8_t[rxBufSize * rxBufCount];
        m_txBuff = new uint8_t[txBufSize * txBufCount];

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

void Ethernet::setMacAddress(const MacAddress_t &mac)
{
    /* set MAC hardware address length */
    m_netif.hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    for (int i=0; i<ETHARP_HWADDR_LEN; i++)
        m_netif.hwaddr[i] = mac.b[i];

    /* initialize MAC address in ethernet MAC */ 
    ETH_MACAddressConfig(ETH_MAC_Address0, m_netif.hwaddr); 
}

const Ethernet::MacAddress_t &Ethernet::macAddress()
{
    return *reinterpret_cast<const MacAddress_t *>(m_netif.hwaddr);
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

void Ethernet::setAddress(const char *ipaddr, const char *netmask, const char *gateway)
{
    ip_addr_t addr, mask, gw;
    addr = ipFromString(ipaddr);
    mask = ipFromString(netmask);
    gw = ipFromString(gateway);
    
    netif_set_addr(&m_netif, &addr, &mask, &gw);
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
    err_t err;
    /* check if any packet received */
    while (ETH_CheckFrameReceived())
    { 
        /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
        
        /* move received packet into a new pbuf */
        struct pbuf *p = low_level_input();

        /* no packet could be read, silently ignore this */
        if (p == NULL)
        {
            err = ERR_MEM;
            break;
        }

        /* entry point to the LwIP stack */
        err = m_netif.input(p, &m_netif);

        if (err != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
struct pbuf * Ethernet::low_level_input()
{
  struct pbuf *p, *q;
  u16_t len;
  int l = 0;
  FrameTypeDef frame;
  uint8_t *buffer;
  uint32_t i=0;
  __IO ETH_DMADESCTypeDef *DMARxNextDesc;
  
  p = NULL;
  
  /* get received frame */
  frame = ETH_Get_Received_Frame();
  
  /* Obtain the size of the packet and put it into the "len" variable. */
  len = frame.length;
  buffer = (uint8_t *)frame.buffer;
  
  /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  
  /* copy received frame to pbuf chain */
  if (p != NULL)
  {
    for (q = p; q != NULL; q = q->next)
    {
      memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
      l = l + q->len;
    }    
  }
  
  /* Release descriptors to DMA */
  /* Check if frame with multiple DMA buffer segments */
  if (DMA_RX_FRAME_infos->Seg_Count > 1)
  {
    DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
  }
  else
  {
    DMARxNextDesc = frame.descriptor;
  }
  
  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
  for (i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++)
  {  
    DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
    DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
  }
  
  /* Clear Segment_Count */
  DMA_RX_FRAME_infos->Seg_Count =0;
  
  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)  
  {
    /* Clear RBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    ETH->DMARPDR = 0;
  }
  return p;
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
err_t Ethernet::low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int framelength = 0;
  uint8_t *buffer =  (uint8_t *)(DMATxDescToSet->Buffer1Addr);
  
  /* copy frame from pbufs to driver buffers */
  for(q = p; q != NULL; q = q->next) 
  {
    memcpy((u8_t*)&buffer[framelength], q->payload, q->len);
	framelength = framelength + q->len;
  }
  
  /* Note: padding and CRC for transmitted frame 
     are automatically inserted by DMA */

  /* Prepare transmit descriptors to give to DMA*/ 
  ETH_Prepare_Transmit_Descriptors(framelength);

  return ERR_OK;
}


err_t Ethernet::ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
  
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = 's';//IFNAME0;
    netif->name[1] = 't';//IFNAME1;
    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    m_self->setMacAddress(defaultMacAddress);

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    netif->flags |= NETIF_FLAG_IGMP;

    /* Initialize Tx Descriptors list: Chain Mode */
    ETH_DMATxDescChainInit(m_self->m_DMATxDscrTab, m_self->m_txBuff, txBufCount);
    /* Initialize Rx Descriptors list: Chain Mode  */
    ETH_DMARxDescChainInit(m_self->m_DMARxDscrTab, m_self->m_rxBuff, rxBufCount);
  
#ifdef CHECKSUM_BY_HARDWARE
    /* Enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
    for(int i=0; i<txBufCount; i++)
    {
        ETH_DMATxDescChecksumInsertionConfig(m_self->m_DMATxDscrTab + i, ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
#endif

    /* Note: TCP, UDP, ICMP checksum checking for received frame are enabled in DMA config */

    /* Enable MAC and DMA transmission and reception */
    ETH_Start();

    // re-set IGMP flag
    netif->flags |= NETIF_FLAG_IGMP;

    return ERR_OK;
}



bool Ethernet::ethConfig(uint16_t phyAddress)
{
    ETH_InitTypeDef ETH_InitStructure;

    /* Enable ETHERNET clock  */
    rcc().setPeriphEnabled(ETH);
                        
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
