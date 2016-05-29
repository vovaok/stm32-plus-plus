#include "usbdevdrv.h"

using namespace Usb;

#define USB_OTG_EP_CONTROL                       0
#define USB_OTG_EP_ISOC                          1
#define USB_OTG_EP_BULK                          2
#define USB_OTG_EP_INT                           3
#define USB_OTG_EP_MASK                          3

/*  Device Status */
#define USB_OTG_DEFAULT                          1
#define USB_OTG_ADDRESSED                        2
#define USB_OTG_CONFIGURED                       3
#define USB_OTG_SUSPENDED                        4

#define CLEAR_IN_EP_INTR(epnum,intr) do { \
  diepint.d32=0; \
  diepint.b.intr = 1; \
  USB_OTG_WRITE_REG32(&pdev->regs.INEP_REGS[epnum]->DIEPINT,diepint.d32); } \
  while (0)

#define CLEAR_OUT_EP_INTR(epnum,intr) do { \
  doepint.d32=0; \
  doepint.b.intr = 1; \
  USB_OTG_WRITE_REG32(&pdev->regs.OUTEP_REGS[epnum]->DOEPINT,doepint.d32); } \
  while (0)
//---------------------------------------------------------------------------

UsbDeviceDriver::UsbDeviceDriver(USB_OTG_CORE_HANDLE *handle)
{
    pdev = handle;
}

UsbDeviceDriver::~UsbDeviceDriver()
{
}
//---------------------------------------------------------------------------

void UsbDeviceDriver::isrHandler()
{
    USB_OTG_GINTSTS_TypeDef gintr_status;
    uint32_t retval = 0;

    if (USB_OTG_IsDeviceMode(pdev)) // ensure that we are in device mode
    {
        gintr_status.d32 = USB_OTG_ReadCoreItr(pdev);
        if (!gintr_status.d32) // avoid spurious interrupt
            return;
        if (gintr_status.b.outepintr)
            retval |= handleOutEp();   
        if (gintr_status.b.inepint)
            retval |= handleInEp();
        if (gintr_status.b.modemismatch)
        {
            USB_OTG_GINTSTS_TypeDef gintsts;
            // clear interrupt 
            gintsts.d32 = 0;
            gintsts.b.modemismatch = 1;
            USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTSTS, gintsts.d32);
        }
        if (gintr_status.b.wkupintr)
            retval |= handleResume();
        if (gintr_status.b.usbsuspend)
            retval |= handleUsbSuspend();
        if (gintr_status.b.sofintr)
            retval |= handleSof();
        if (gintr_status.b.rxstsqlvl)
            retval |= handleRxStatusQueueLevel();
        if (gintr_status.b.usbreset)
            retval |= handleUsbReset();
        if (gintr_status.b.enumdone)
            retval |= handleEnumDone();
        if (gintr_status.b.incomplisoin)
            retval |= handleIsoInIncomplete();
        if (gintr_status.b.incomplisoout)
            retval |= handleIsoOutIncomplete();   
        #ifdef VBUS_SENSING_ENABLED
        if (gintr_status.b.sessreqintr)
            retval |= sessionRequest();
        if (gintr_status.b.otgintr)
            retval |= otgIsr();   
        #endif    
    }
    // do something with retval
}
//---------------------------------------------------------------------------

uint32_t UsbDeviceDriver::readDevInEp(uint8_t epnum)
{
    uint32_t v, msk, emp;
    msk = USB_OTG_READ_REG32(&pdev->regs.DREGS->DIEPMSK);
    emp = USB_OTG_READ_REG32(&pdev->regs.DREGS->DIEPEMPMSK);
    msk |= ((emp >> epnum) & 0x1) << 7;
    v = USB_OTG_READ_REG32(&pdev->regs.INEP_REGS[epnum]->DIEPINT) & msk;
    return v;
}
//---------------------------------------------------------------------------

uint32_t UsbDeviceDriver::handleInEp()
{ 
    USB_OTG_DIEPINTn_TypeDef  diepint;
    uint32_t ep_intr;
    uint32_t epnum = 0;
    uint32_t fifoemptymsk;
    diepint.d32 = 0;
    ep_intr = USB_OTG_ReadDevAllInEPItr(pdev);
    while ( ep_intr )
    {
        if (ep_intr&0x1) /* In ITR */
        {
            diepint.d32 = readDevInEp(epnum); /* Get In ITR status */
            if ( diepint.b.xfercompl )
            {
                fifoemptymsk = 0x1 << epnum;
                USB_OTG_MODIFY_REG32(&pdev->regs.DREGS->DIEPEMPMSK, fifoemptymsk, 0);
                CLEAR_IN_EP_INTR(epnum, xfercompl);
                /* TX COMPLETE */
                
                dataInStageEvent(epnum); // this is EVENT!!
                
                if (pdev->cfg.dma_enable == 1)
                {
                    if((epnum == 0) && (pdev->dev.device_state == USB_OTG_EP0_STATUS_IN))                        
                        USB_OTG_EP0_OutStart(pdev); // prepare to rx more setup packets
                }           
            }
            if ( diepint.b.timeout )
                CLEAR_IN_EP_INTR(epnum, timeout);
            if (diepint.b.intktxfemp)
                CLEAR_IN_EP_INTR(epnum, intktxfemp);
            if (diepint.b.inepnakeff)
                CLEAR_IN_EP_INTR(epnum, inepnakeff);
            if ( diepint.b.epdisabled )
                CLEAR_IN_EP_INTR(epnum, epdisabled);    
            if (diepint.b.emptyintr)
            {
                writeEmptyTxFifo(epnum);
                CLEAR_IN_EP_INTR(epnum, emptyintr);
            }
        }
        epnum++;
        ep_intr >>= 1;
    }
    
    
    return 1;
}

uint32_t UsbDeviceDriver::handleOutEp()
{
    uint32_t ep_intr;
    USB_OTG_DOEPINTn_TypeDef  doepint;
    USB_OTG_DEPXFRSIZ_TypeDef  deptsiz;
    uint32_t epnum = 0;
    doepint.d32 = 0;
    /* Read in the device interrupt bits */
    ep_intr = USB_OTG_ReadDevAllOutEp_itr(pdev);
    while ( ep_intr )
    {
        if (ep_intr&0x1)
        {
            doepint.d32 = USB_OTG_ReadDevOutEP_itr(pdev, epnum);
            /* Transfer complete */
            if ( doepint.b.xfercompl )
            {
                /* Clear the bit in DOEPINTn for this interrupt */
                CLEAR_OUT_EP_INTR(epnum, xfercompl);
                if (pdev->cfg.dma_enable == 1)
                {
                    deptsiz.d32 = USB_OTG_READ_REG32(&(pdev->regs.OUTEP_REGS[epnum]->DOEPTSIZ));
                    /*ToDo : handle more than one single MPS size packet */
                    pdev->dev.out_ep[epnum].xfer_count = pdev->dev.out_ep[epnum].maxpacket - \
                    deptsiz.b.xfersize;
                }
                /* Inform upper layer: data ready */
                /* RX COMPLETE */
                dataOutStageEvent(epnum); // this is EVENT!!

                if (pdev->cfg.dma_enable == 1)
                {
                    if((epnum == 0) && (pdev->dev.device_state == USB_OTG_EP0_STATUS_OUT))
                        USB_OTG_EP0_OutStart(pdev); // prepare to rx more setup packets
                }        
            }
            /* Endpoint disable  */
            if ( doepint.b.epdisabled )
            {
                /* Clear the bit in DOEPINTn for this interrupt */
                CLEAR_OUT_EP_INTR(epnum, epdisabled);
            }
            /* Setup Phase Done (control EPs) */
            if ( doepint.b.setup )
            {
                /* inform the upper layer that a setup packet is available */
                /* SETUP COMPLETE */
                setupStageEvent(); // this is EVENT!!
                CLEAR_OUT_EP_INTR(epnum, setup);
            }
        }
        epnum++;
        ep_intr >>= 1;
    }
    return 1;
}

uint32_t UsbDeviceDriver::handleSof()
{
    USB_OTG_GINTSTS_TypeDef  GINTSTS;
    sofEvent(); // this is EVENT
    // Clear interrupt */
    GINTSTS.d32 = 0;
    GINTSTS.b.sofintr = 1;
    USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GINTSTS, GINTSTS.d32);
    return 1;
}
//---------------------------------------------------------------------------

uint32_t UsbDeviceDriver::handleRxStatusQueueLevel()
{
    USB_OTG_GINTMSK_TypeDef  int_mask;
    USB_OTG_DRXSTS_TypeDef   status;
    USB_OTG_EP *ep;

    /* Disable the Rx Status Queue Level interrupt */
    int_mask.d32 = 0;
    int_mask.b.rxstsqlvl = 1;
    USB_OTG_MODIFY_REG32( &pdev->regs.GREGS->GINTMSK, int_mask.d32, 0);

    /* Get the Status from the top of the FIFO */
    status.d32 = USB_OTG_READ_REG32( &pdev->regs.GREGS->GRXSTSP );

    ep = &pdev->dev.out_ep[status.b.epnum];

    switch (status.b.pktsts)
    {
      case STS_GOUT_NAK:
        break;
      case STS_DATA_UPDT:
        if (status.b.bcnt)
        {
            USB_OTG_ReadPacket(pdev, ep->xfer_buff, status.b.bcnt);
            ep->xfer_buff += status.b.bcnt;
            ep->xfer_count += status.b.bcnt;
        }
        break;
      case STS_XFER_COMP:
        break;
      case STS_SETUP_COMP:
        break;
      case STS_SETUP_UPDT:
        /* Copy the setup packet received in FIFO into the setup buffer in RAM */
        USB_OTG_ReadPacket(pdev, pdev->dev.setup_packet, 8);
        ep->xfer_count += status.b.bcnt;
        break;
      default:
        break;
    }
    
    /* Enable the Rx Status Queue Level interrupt */
    USB_OTG_MODIFY_REG32( &pdev->regs.GREGS->GINTMSK, 0, int_mask.d32);
    return 1;
}

uint32_t UsbDeviceDriver::writeEmptyTxFifo(uint32_t epnum)
{
    USB_OTG_DTXFSTSn_TypeDef txstatus;
    USB_OTG_EP *ep;
    uint32_t len = 0;
    uint32_t len32b;
    txstatus.d32 = 0;

    ep = &pdev->dev.in_ep[epnum];    
    len = ep->xfer_len - ep->xfer_count;

    if (len > ep->maxpacket)
        len = ep->maxpacket;

    len32b = (len + 3) >> 2;
    txstatus.d32 = USB_OTG_READ_REG32( &pdev->regs.INEP_REGS[epnum]->DTXFSTS);

    while (txstatus.b.txfspcavail > len32b &&
           ep->xfer_count < ep->xfer_len &&
           ep->xfer_len != 0)
    {
        /* Write the FIFO */
        len = ep->xfer_len - ep->xfer_count;
        if (len > ep->maxpacket)
            len = ep->maxpacket;
        len32b = (len + 3) >> 2;
        USB_OTG_WritePacket(pdev, ep->xfer_buff, epnum, len);
        ep->xfer_buff  += len;
        ep->xfer_count += len;
        txstatus.d32 = USB_OTG_READ_REG32(&pdev->regs.INEP_REGS[epnum]->DTXFSTS);
    }
    
    // dopisano!!
    if (ep->xfer_count >= ep->xfer_len)
    {
        // if all is done, disable interrupt
        uint32_t fifoemptymsk = 1 << ep->num;
        USB_OTG_MODIFY_REG32(&pdev->regs.DREGS->DIEPEMPMSK, fifoemptymsk, 0);
    }
    return 1;
}
//---------------------------------------------------------------------------

uint32_t UsbDeviceDriver::handleUsbReset()
{
    USB_OTG_DAINT_TypeDef    daintmsk;
    USB_OTG_DOEPMSK_TypeDef  doepmsk;
    USB_OTG_DIEPMSK_TypeDef  diepmsk;
    USB_OTG_DCFG_TypeDef     dcfg;
    USB_OTG_DCTL_TypeDef     dctl;
    USB_OTG_GINTSTS_TypeDef  gintsts;
    uint32_t i;

    dctl.d32 = 0;
    daintmsk.d32 = 0;
    doepmsk.d32 = 0;
    diepmsk.d32 = 0;
    dcfg.d32 = 0;
    gintsts.d32 = 0;

    /* Clear the Remote Wake-up Signaling */
    dctl.b.rmtwkupsig = 1;
    USB_OTG_MODIFY_REG32(&pdev->regs.DREGS->DCTL, dctl.d32, 0);

    /* Flush the Tx FIFO */
    USB_OTG_FlushTxFifo(pdev, 0);

    for (i = 0; i < pdev->cfg.dev_endpoints ; i++)
    {
        USB_OTG_WRITE_REG32( &pdev->regs.INEP_REGS[i]->DIEPINT, 0xFF);
        USB_OTG_WRITE_REG32( &pdev->regs.OUTEP_REGS[i]->DOEPINT, 0xFF);
    }
    USB_OTG_WRITE_REG32( &pdev->regs.DREGS->DAINT, 0xFFFFFFFF );

    daintmsk.ep.in = 1;
    daintmsk.ep.out = 1;
    USB_OTG_WRITE_REG32( &pdev->regs.DREGS->DAINTMSK, daintmsk.d32 );

    doepmsk.b.setup = 1;
    doepmsk.b.xfercompl = 1;
    doepmsk.b.epdisabled = 1;
    USB_OTG_WRITE_REG32( &pdev->regs.DREGS->DOEPMSK, doepmsk.d32 );
    #ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED   
        USB_OTG_WRITE_REG32( &pdev->regs.DREGS->DOUTEP1MSK, doepmsk.d32 );
    #endif
    diepmsk.b.xfercompl = 1;
    diepmsk.b.timeout = 1;
    diepmsk.b.epdisabled = 1;

    USB_OTG_WRITE_REG32( &pdev->regs.DREGS->DIEPMSK, diepmsk.d32 );
    #ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED  
        USB_OTG_WRITE_REG32( &pdev->regs.DREGS->DINEP1MSK, diepmsk.d32 );
    #endif
    /* Reset Device Address */
    dcfg.d32 = USB_OTG_READ_REG32( &pdev->regs.DREGS->DCFG);
    dcfg.b.devaddr = 0;
    USB_OTG_WRITE_REG32( &pdev->regs.DREGS->DCFG, dcfg.d32);

    /* setup EP0 to receive SETUP packets */
    USB_OTG_EP0_OutStart(pdev);

    /* Clear interrupt */
    gintsts.d32 = 0;
    gintsts.b.usbreset = 1;
    USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GINTSTS, gintsts.d32);

    /*Reset internal state machine */
    resetEvent(); // this is EVENT!!
    return 1;
}

uint32_t UsbDeviceDriver::handleEnumDone()
{
    USB_OTG_GINTSTS_TypeDef  gintsts;
    USB_OTG_GUSBCFG_TypeDef  gusbcfg;

    USB_OTG_EP0Activate(pdev);

    /* Set USB turn-around time based on device speed and PHY interface. */
    gusbcfg.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GUSBCFG);

    /* Full or High speed */
    if ( USB_OTG_GetDeviceSpeed(pdev) == USB_SPEED_HIGH)
    {
        pdev->cfg.speed            = USB_OTG_SPEED_HIGH;
        pdev->cfg.mps              = USB_OTG_HS_MAX_PACKET_SIZE ;    
        gusbcfg.b.usbtrdtim = 9;
    }
    else
    {
        pdev->cfg.speed            = USB_OTG_SPEED_FULL;
        pdev->cfg.mps              = USB_OTG_FS_MAX_PACKET_SIZE ;  
        gusbcfg.b.usbtrdtim = 5;
    }

    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GUSBCFG, gusbcfg.d32);

    /* Clear interrupt */
    gintsts.d32 = 0;
    gintsts.b.enumdone = 1;
    USB_OTG_WRITE_REG32( &pdev->regs.GREGS->GINTSTS, gintsts.d32 );
    return 1;
}

uint32_t UsbDeviceDriver::handleResume()
{
    USB_OTG_GINTSTS_TypeDef  gintsts;
    USB_OTG_DCTL_TypeDef     devctl;
    USB_OTG_PCGCCTL_TypeDef  power;

    if (pdev->cfg.low_power)
    {
        /* un-gate USB Core clock */
        power.d32 = USB_OTG_READ_REG32(&pdev->regs.PCGCCTL);
        power.b.gatehclk = 0;
        power.b.stoppclk = 0;
        USB_OTG_WRITE_REG32(pdev->regs.PCGCCTL, power.d32);
    }

    /* Clear the Remote Wake-up Signaling */
    devctl.d32 = 0;
    devctl.b.rmtwkupsig = 1;
    USB_OTG_MODIFY_REG32(&pdev->regs.DREGS->DCTL, devctl.d32, 0);

    /* Inform upper layer by the Resume Event */
    resumeEvent(); // this is EVENT!!

    /* Clear interrupt */
    gintsts.d32 = 0;
    gintsts.b.wkupintr = 1;
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTSTS, gintsts.d32);
    return 1;
}

uint32_t UsbDeviceDriver::handleUsbSuspend()
{
    USB_OTG_GINTSTS_TypeDef  gintsts;
    USB_OTG_PCGCCTL_TypeDef  power;
    USB_OTG_DSTS_TypeDef     dsts;
    __IO uint8_t prev_status = 0;

    prev_status = pdev->dev.device_status;
    suspendEvent(); // this is EVENT!!      

    dsts.d32 = USB_OTG_READ_REG32(&pdev->regs.DREGS->DSTS);

    /* Clear interrupt */
    gintsts.d32 = 0;
    gintsts.b.usbsuspend = 1;
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTSTS, gintsts.d32);

    if ((pdev->cfg.low_power) && (dsts.b.suspsts == 1)  && 
        (pdev->dev.connection_status == 1) && 
        (prev_status  == USB_OTG_CONFIGURED))
    {
        /*  switch-off the clocks */
        power.d32 = 0;
        power.b.stoppclk = 1;
        USB_OTG_MODIFY_REG32(pdev->regs.PCGCCTL, 0, power.d32);  

        power.b.gatehclk = 1;
        USB_OTG_MODIFY_REG32(pdev->regs.PCGCCTL, 0, power.d32);

        /* Request to enter Sleep mode after exit from current ISR */
        SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk);
    }
    return 1;
}
//---------------------------------------------------------------------------

uint32_t UsbDeviceDriver::handleIsoInIncomplete()
{
    USB_OTG_GINTSTS_TypeDef gintsts;  
    gintsts.d32 = 0;
    isoInIncompleteEvent(); // this is EVENT!!
    // clear interrupt
    gintsts.b.incomplisoin = 1;
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTSTS, gintsts.d32);
    return 1;
}

uint32_t UsbDeviceDriver::handleIsoOutIncomplete()
{
    USB_OTG_GINTSTS_TypeDef gintsts;  
    gintsts.d32 = 0;
    isoOutIncompleteEvent(); // this is EVENT!!
    // Clear interrupt
    gintsts.b.incomplisoout = 1;
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTSTS, gintsts.d32);
    return 1;
}
//---------------------------------------------------------------------------

#ifdef VBUS_SENSING_ENABLED
uint32_t UsbDeviceDriver::sessionRequest()
{
    USB_OTG_GINTSTS_TypeDef gintsts;  
    USBD_DCD_INT_fops->DevConnected(pdev);
    // clear interrupt
    gintsts.d32 = 0;
    gintsts.b.sessreqintr = 1;
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTSTS, gintsts.d32);   
    return 1;
}

uint32_t UsbDeviceDriver::otgIsr()
{
    USB_OTG_GOTGINT_TypeDef gotgint;
    gotgint.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GOTGINT);
    if (gotgint.b.sesenddet)
        devDisconnectedEvent(); // this is EVENT!!
    // Clear OTG interrupt 
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GOTGINT, gotgint.d32); 
    return 1;
}
#endif
//---------------------------------------------------------------------------

void UsbDeviceDriver::init(USB_OTG_CORE_ID_TypeDef coreId)
{
    uint32_t i;
    USB_OTG_EP *ep;

    USB_OTG_SelectCore(pdev, coreId);

    pdev->dev.device_status = USB_OTG_DEFAULT;
    pdev->dev.device_address = 0;

    /* Init ep structure */
    for (i = 0; i < pdev->cfg.dev_endpoints ; i++)
    {
        ep = &pdev->dev.in_ep[i];
        /* Init ep structure */
        ep->is_in = 1;
        ep->num = i;
        ep->tx_fifo_num = i;
        /* Control until ep is actvated */
        ep->type = EP_TYPE_CTRL;
        ep->maxpacket =  USB_OTG_MAX_EP0_SIZE;
        ep->xfer_buff = 0;
        ep->xfer_len = 0;
    }

    for (i = 0; i < pdev->cfg.dev_endpoints; i++)
    {
        ep = &pdev->dev.out_ep[i];
        /* Init ep structure */
        ep->is_in = 0;
        ep->num = i;
        ep->tx_fifo_num = i;
        /* Control until ep is activated */
        ep->type = EP_TYPE_CTRL;
        ep->maxpacket = USB_OTG_MAX_EP0_SIZE;
        ep->xfer_buff = 0;
        ep->xfer_len = 0;
    }

    USB_OTG_DisableGlobalInt(pdev);
    /*Init the Core (common init.) */
    USB_OTG_CoreInit(pdev);
    /* Force Device Mode*/
    USB_OTG_SetCurrentMode(pdev, DEVICE_MODE);
    /* Init Device */
    USB_OTG_CoreInitDev(pdev);
    /* Enable USB Global interrupt */
    USB_OTG_EnableGlobalInt(pdev);
}

void UsbDeviceDriver::devConnect()
{
#ifndef USE_OTG_MODE
    USB_OTG_DCTL_TypeDef  dctl;
    dctl.d32 = USB_OTG_READ_REG32(&pdev->regs.DREGS->DCTL);
    /* Connect device */
    dctl.b.sftdiscon  = 0;
    USB_OTG_WRITE_REG32(&pdev->regs.DREGS->DCTL, dctl.d32);
    USB_OTG_BSP_mDelay(3);
#endif
}

void UsbDeviceDriver::devDisconnect()
{
#ifndef USE_OTG_MODE
    USB_OTG_DCTL_TypeDef  dctl;
    dctl.d32 = USB_OTG_READ_REG32(&pdev->regs.DREGS->DCTL);
    /* Disconnect device for 3ms */
    dctl.b.sftdiscon  = 1;
    USB_OTG_WRITE_REG32(&pdev->regs.DREGS->DCTL, dctl.d32);
    USB_OTG_BSP_mDelay(3);
#endif
}

void UsbDeviceDriver::epSetAddress(uint8_t address)
{
    USB_OTG_DCFG_TypeDef  dcfg;
    dcfg.d32 = 0;
    dcfg.b.devaddr = address;
    USB_OTG_MODIFY_REG32( &pdev->regs.DREGS->DCFG, 0, dcfg.d32);
}

uint32_t UsbDeviceDriver::epOpen(uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type)
{
    USB_OTG_EP *ep;
    if ((ep_addr & 0x80) == 0x80)
        ep = &pdev->dev.in_ep[ep_addr & 0x7F];
    else
        ep = &pdev->dev.out_ep[ep_addr & 0x7F];
    ep->num   = ep_addr & 0x7F;
    ep->is_in = (0x80 & ep_addr) != 0;
    ep->maxpacket = ep_mps;
    ep->type = ep_type;
    if (ep->is_in)
        ep->tx_fifo_num = ep->num; // Assign a Tx FIFO
    /* Set initial data PID. */
    if (ep_type == USB_OTG_EP_BULK )
        ep->data_pid_start = 0;
    USB_OTG_EPActivate(pdev, ep);
    return 0;
}

uint32_t UsbDeviceDriver::epClose(uint8_t ep_addr)
{
    USB_OTG_EP *ep;
    if ((ep_addr&0x80) == 0x80)
        ep = &pdev->dev.in_ep[ep_addr & 0x7F];
    else
        ep = &pdev->dev.out_ep[ep_addr & 0x7F];
    ep->num   = ep_addr & 0x7F;
    ep->is_in = (0x80 & ep_addr) != 0;
    USB_OTG_EPDeactivate(pdev, ep);
    return 0;
}

uint32_t UsbDeviceDriver::epPrepareRx(uint8_t ep_addr, void *pbuf, uint16_t buf_len)
{
    USB_OTG_EP *ep;
    ep = &pdev->dev.out_ep[ep_addr & 0x7F];
    /*setup and start the Xfer */
    ep->xfer_buff = reinterpret_cast<uint8_t*>(pbuf);  
    ep->xfer_len = buf_len;
    ep->xfer_count = 0;
    ep->is_in = 0;
    ep->num = ep_addr & 0x7F;
    if (pdev->cfg.dma_enable == 1)
        ep->dma_addr = (uint32_t)pbuf;
    if ( ep->num == 0 )
        USB_OTG_EP0StartXfer(pdev, ep);
    else
        USB_OTG_EPStartXfer(pdev, ep);
    return 0;
}

uint32_t UsbDeviceDriver::epTx(uint8_t ep_addr, void *pbuf, uint32_t buf_len)
{
    USB_OTG_EP *ep;
    ep = &pdev->dev.in_ep[ep_addr & 0x7F];
    /* Setup and start the Transfer */
    ep->is_in = 1;
    ep->num = ep_addr & 0x7F;  
    ep->xfer_buff = reinterpret_cast<uint8_t*>(pbuf);
    ep->dma_addr = (uint32_t)pbuf;  
    ep->xfer_count = 0;
    ep->xfer_len  = buf_len;
    if ( ep->num == 0 )
        USB_OTG_EP0StartXfer(pdev, ep);
    else
        USB_OTG_EPStartXfer(pdev, ep);    
    return 0;
}

uint32_t UsbDeviceDriver::epStall(uint8_t epnum)
{
    USB_OTG_EP *ep;
    if ((0x80 & epnum) == 0x80)
        ep = &pdev->dev.in_ep[epnum & 0x7F];
    else
        ep = &pdev->dev.out_ep[epnum];
    ep->is_stall = 1;
    ep->num   = epnum & 0x7F;
    ep->is_in = ((epnum & 0x80) == 0x80);
    USB_OTG_EPSetStall(pdev, ep);
    return (0);
}

uint32_t UsbDeviceDriver::epClrStall(uint8_t epnum)
{
    USB_OTG_EP *ep;
    if ((0x80 & epnum) == 0x80)
        ep = &pdev->dev.in_ep[epnum & 0x7F]; 
    else
        ep = &pdev->dev.out_ep[epnum];
    ep->is_stall = 0;  
    ep->num   = epnum & 0x7F;
    ep->is_in = ((epnum & 0x80) == 0x80);
    USB_OTG_EPClearStall(pdev, ep);
    return (0);
}

uint32_t UsbDeviceDriver::epFlush(uint8_t epnum)
{
    if ((epnum & 0x80) == 0x80)
        USB_OTG_FlushTxFifo(pdev, epnum & 0x7F);
    else
        USB_OTG_FlushRxFifo(pdev);
    return (0);
}

uint32_t UsbDeviceDriver::getEpStatus(uint8_t epnum)
{
    USB_OTG_EP *ep;
    uint32_t Status = 0;
    if ((0x80 & epnum) == 0x80)
        ep = &pdev->dev.in_ep[epnum & 0x7F];    
    else
        ep = &pdev->dev.out_ep[epnum];
    Status = USB_OTG_GetEPStatus(pdev, ep);
    return Status;
}

void UsbDeviceDriver::setEpStatus(uint8_t epnum, uint32_t Status)
{
    USB_OTG_EP *ep;
    if ((0x80 & epnum) == 0x80)
        ep = &pdev->dev.in_ep[epnum & 0x7F];
    else
        ep = &pdev->dev.out_ep[epnum];
    USB_OTG_SetEPStatus(pdev, ep, Status);
}
//---------------------------------------------------------------------------

void UsbDeviceDriver::ctlSendData(const ByteArray &ba)
{
    mTxBuffer = ba;
    pdev->dev.in_ep[0].total_data_len = mTxBuffer.length();
    pdev->dev.in_ep[0].rem_data_len   = mTxBuffer.length();
    pdev->dev.device_state = USB_OTG_EP0_DATA_IN;
    epTx(0, mTxBuffer.data(), mTxBuffer.length());
}

void UsbDeviceDriver::ctlSendData(void *pbuf, unsigned short len)
{  
    mTxBuffer.clear();
    mTxBuffer.append(pbuf, len);
    pdev->dev.in_ep[0].total_data_len = mTxBuffer.length();
    pdev->dev.in_ep[0].rem_data_len   = mTxBuffer.length();
    pdev->dev.device_state = USB_OTG_EP0_DATA_IN;
    epTx(0, mTxBuffer.data(), mTxBuffer.length());
}

void UsbDeviceDriver::ctlContinueSendData(void *pbuf, unsigned short len)
{
    epTx(0, pbuf, len);
//    mTxBuffer.clear();
//    mTxBuffer.append(pbuf, len);
//    epTx(0, mTxBuffer.data(), mTxBuffer.length());
}

void UsbDeviceDriver::ctlError(uint8_t ep_direction)
{
    epStall(ep_direction & 0x80);
    USB_OTG_EP0_OutStart(pdev);  
}

void UsbDeviceDriver::ctlSendStatus()
{
    pdev->dev.device_state = USB_OTG_EP0_STATUS_IN;
    epTx(0, 0L, 0);
    USB_OTG_EP0_OutStart(pdev);
}

void UsbDeviceDriver::ctlReceiveStatus()
{
    pdev->dev.device_state = USB_OTG_EP0_STATUS_OUT;  
    epPrepareRx(0, 0L, 0);  
    USB_OTG_EP0_OutStart(pdev);
}

void UsbDeviceDriver::ctlReceiveData(void *pbuf, unsigned short len)
{
    pdev->dev.out_ep[0].total_data_len = len;
    pdev->dev.out_ep[0].rem_data_len   = len;
    pdev->dev.device_state = USB_OTG_EP0_DATA_OUT;
    epPrepareRx(0, pbuf, len);
}

void UsbDeviceDriver::ctlContinueRx(void *pbuf, unsigned short len)
{
    epPrepareRx(0, pbuf, len);
}
//---------------------------------------------------------------------------