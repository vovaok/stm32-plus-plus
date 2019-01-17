#include "usbdev.h"

using namespace Usb;

UsbDevice* UsbDevice::mFsCore = 0L;
UsbDevice* UsbDevice::mHsCore = 0L;

UsbDevice::UsbDevice(UsbCore core) :
  UsbNode(UsbNode::NodeTypeDevice),
    mUsbCore(core),
    mDefaultConfig(0),
    mConfigIdx(0),
    mCurrentConfig(0L)
{  
    memset(&mDev, 0, sizeof(USB_OTG_CORE_HANDLE));
    mTestMode.d32 = 0;
  
    if (mUsbCore == OtgFs)
        mFsCore = this;
    else if (mUsbCore == OtgHs || mUsbCore == OtgHsUlpi)
        mHsCore = this;
    else
        throw Exception::invalidPeriph;
    
    mDeviceDescriptor = new DeviceDescriptor();
    assignDescriptor(mDeviceDescriptor);
    
    if (mUsbCore == OtgHsUlpi)
        mDeviceDescriptor->setUsbSpecification(0x0200); // USB2.0
    
    mStrings.push_back(LangIdDescriptor(USBD_LANGID_STRING).toByteArray()); // 0
    mStrings.push_back(StringDescriptor(USBD_MANUFACTURER_STRING).toByteArray()); // 1
    mStrings.push_back(StringDescriptor(USBD_PRODUCT_STRING).toByteArray()); // 2
    mStrings.push_back(StringDescriptor(USBD_SERIAL_NUMBER).toByteArray()); //3
    
    // hardware init
    bspInit();

//    /*Register class and user callbacks */
//    pdev->dev.class_cb = class_cb;
//    pdev->dev.usr_cb = usr_cb;  
//    pdev->dev.usr_device = pDevice;    

    // set USB OTG core params
    mDrv = new UsbDeviceDriver(&mDev);
    mDrv->setupStageEvent = EVENT(&UsbDevice::setupStage);
    mDrv->dataOutStageEvent = EVENT(&UsbDevice::dataOutStage);
    mDrv->dataInStageEvent = EVENT(&UsbDevice::dataInStage);
    mDrv->sofEvent = EVENT(&UsbDevice::sofHandler);
    mDrv->resetEvent = EVENT(&UsbDevice::reset);
    mDrv->suspendEvent = EVENT(&UsbDevice::suspend);
    mDrv->resumeEvent = EVENT(&UsbDevice::resume);
    mDrv->isoInIncompleteEvent = EVENT(&UsbDevice::isoInIncompleteHandler);
    mDrv->isoOutIncompleteEvent = EVENT(&UsbDevice::isoOutIncompleteHandler);
    #ifdef VBUS_SENSING_ENABLED
    mDrv->devConnectedEvent = EVENT(&UsbDevice::devConnected);
    mDrv->devDisconnectedEvent = EVENT(&UsbDevice::devDisconnected);
    #endif
    // further init will be performed in start();
}

UsbDevice::~UsbDevice()
{
    if (mUsbCore == OtgFs)
        mFsCore = 0L;
    else if (mUsbCore == OtgHs || mUsbCore == OtgHsUlpi)
        mHsCore = 0L;
    //USBD_DeInit(&mDev); 
    delete mDrv;
}

void UsbDevice::addChild(UsbNode* configuration)
{
    UsbConfiguration *config = dynamic_cast<UsbConfiguration*>(configuration);
    if (config)
    {
        unsigned char numCfg = mDeviceDescriptor->numConfigurations();
        mDeviceDescriptor->setNumConfigurations(++numCfg);
        config->setConfigurationValue(numCfg); // configuration 00 is null
    }
    UsbNode::addChild(configuration);
}
//---------------------------------------------------------------------------

void UsbDevice::start()
{
    USB_OTG_CORE_ID_TypeDef coreId = (mUsbCore == OtgFs)? USB_OTG_FS_CORE_ID: USB_OTG_HS_CORE_ID;
    mDrv->init(coreId);
    // enable interrupt
    bspEnableInterrupt();
}
//---------------------------------------------------------------------------

void UsbDevice::setDeviceClass(unsigned char devClass, unsigned char subClass, unsigned char protocol)
{
    mDeviceDescriptor->setDeviceClass(devClass);
    mDeviceDescriptor->setDeviceSubClass(subClass);
    mDeviceDescriptor->setDeviceProtocol(protocol);
}

void UsbDevice::setMaxPacketSize(unsigned char size)
{
    mDeviceDescriptor->setMaxPacketSize0(size);
}

void UsbDevice::setVidPid(unsigned short VID, unsigned short PID)
{
    mDeviceDescriptor->setVendorId(VID);
    mDeviceDescriptor->setProductId(PID);
}

void UsbDevice::setDeviceRevisionNumber(unsigned char majorVersion, unsigned char minorVersion)
{
    mDeviceDescriptor->setDeviceRevisionNumber((((unsigned short)majorVersion) << 8) | minorVersion);
}

void UsbDevice::setLangId(unsigned short langId)
{
    mStrings[0] = LangIdDescriptor(langId).toByteArray();
}

void UsbDevice::setManufacturer(const string &manufacturer)
{
    mStrings[1] = StringDescriptor(manufacturer).toByteArray();
}

void UsbDevice::setManufacturer(const wstring &manufacturer)
{
    mStrings[1] = StringDescriptor(manufacturer).toByteArray();
}

void UsbDevice::setProduct(const string &product)
{
    mStrings[2] = StringDescriptor(product).toByteArray();
}

void UsbDevice::setProduct(const wstring &product)
{
    mStrings[2] = StringDescriptor(product).toByteArray();
}

void UsbDevice::setSerialNumber(const string &serial)
{
    mStrings[3] = StringDescriptor(serial).toByteArray();
}

void UsbDevice::setSerialNumber(const wstring &serial)
{
    mStrings[3] = StringDescriptor(serial).toByteArray();
}

void UsbDevice::setUsbSpecification(unsigned short spec)
{
    mDeviceDescriptor->setUsbSpecification(spec);
}
//---------------------------------------------------------------------------

void UsbDevice::setTestMode(USB_OTG_DCTL_TypeDef dctl)
{
    mTestMode.d32 = dctl.d32;
}

void UsbDevice::runTestMode()
{
    USB_OTG_WRITE_REG32(&mDev.regs.DREGS->DCTL, mTestMode.d32);
}

void UsbDevice::setupStage()
{
    UsbSetupReq req = *reinterpret_cast<UsbSetupReq*>(&mDev.dev.setup_packet);
    mDev.dev.in_ep[0].ctl_data_len = req.wLength;
    mDev.dev.device_state = USB_OTG_EP0_SETUP;
//    printf("req 0x%02X\n", req.bmRequest);
    switch (req.bmRequest & 0x1F)
    {
        case USB_REQ_RECIPIENT_DEVICE: handleStdReq(req); break;
        case USB_REQ_RECIPIENT_INTERFACE: handleStdItfReq(req); break;
        case USB_REQ_RECIPIENT_ENDPOINT: handleStdEpReq(req); break;
        default: mDrv->epStall(req.bmRequest & 0x80); // oO wtf? Hy u nox.
    }
}

void UsbDevice::dataOutStage(unsigned char epnum)
{
    USB_OTG_EP *ep;
  
    if (epnum == 0) 
    {
        ep = &mDev.dev.out_ep[0];
        if (mDev.dev.device_state == USB_OTG_EP0_DATA_OUT)
        {
            if(ep->rem_data_len > ep->maxpacket)
            {
                ep->rem_data_len -= ep->maxpacket;
                if(mDev.cfg.dma_enable == 1)
                {
                  /* in slave mode this, is handled by the RxSTSQLvl ISR */
                    ep->xfer_buff += ep->maxpacket; 
                }        
                mDrv->ctlContinueRx(ep->xfer_buff, MIN(ep->rem_data_len ,ep->maxpacket));
            }
            else
            {
                if ((mCurrentConfig) && (mDev.dev.device_status == UsbOtgConfigured))
                    mCurrentConfig->ep0RxReady();
                mDrv->ctlSendStatus();
            }
        }
    }
    else if (mEndpoints.count(epnum) && (mDev.dev.device_status == UsbOtgConfigured))
    {
        mEndpoints[epnum]->dataOut(mDev.dev.out_ep[epnum].xfer_count);
    }
}

void UsbDevice::dataInStage(unsigned char epnum)
{
//    printf("data IN stage EP=0x%02X\n", epnum);
    USB_OTG_EP *ep;  
    if (epnum == 0) 
    {
        ep = &mDev.dev.in_ep[0];
        if (mDev.dev.device_state == USB_OTG_EP0_DATA_IN)
        {
            if (ep->rem_data_len > ep->maxpacket)
            {
                ep->rem_data_len -= ep->maxpacket;
                if (mDev.cfg.dma_enable == 1)
                {
                    /* in slave mode this, is handled by the TxFifoEmpty ISR */
                    ep->xfer_buff += ep->maxpacket;
                }
                mDrv->ctlContinueSendData(ep->xfer_buff, ep->rem_data_len);
            }
            else
            { /* last packet is MPS multiple, so send ZLP packet */
                if ((ep->total_data_len % ep->maxpacket == 0) &&
                    (ep->total_data_len >= ep->maxpacket) &&
                    (ep->total_data_len < ep->ctl_data_len))
                {
                    mDrv->ctlContinueSendData(0L, 0);
                    ep->ctl_data_len = 0;
                }
                else
                {
                    if ((mCurrentConfig) && (mDev.dev.device_status == UsbOtgConfigured))
                        mCurrentConfig->ep0TxSent();
                    mDrv->ctlReceiveStatus();
                }
            }
        }
        if (mDev.dev.test_mode == 1)
        {
            runTestMode();
            mDev.dev.test_mode = 0;
        }
    }
    else
    {
        epnum |= 0x80;
        if (mEndpoints.count(epnum) && (mDev.dev.device_status == UsbOtgConfigured))
        {
            mEndpoints[epnum]->dataIn();
        }
    }
    
//    else if ((mCurrentConfig) && (mDev.dev.device_status == UsbOtgConfigured))
//    {
//        mCurrentConfig->dataIn(epnum);
//    }  
}

void UsbDevice::sofHandler()
{
    if (mCurrentConfig)
        mCurrentConfig->sof();
}

void UsbDevice::reset()
{
    /* Open EP0 OUT */
    mDrv->epOpen(0x00, USB_OTG_MAX_EP0_SIZE, EP_TYPE_CTRL);
    /* Open EP0 IN */
    mDrv->epOpen(0x80, USB_OTG_MAX_EP0_SIZE, EP_TYPE_CTRL);
    /* Upon Reset call usr call back */
    mDev.dev.device_status = UsbOtgDefault;
    //pdev->dev.usr_cb->DeviceReset(pdev->cfg.speed);
}

void UsbDevice::suspend()
{
    mDev.dev.device_old_status = mDev.dev.device_status;
    mDev.dev.device_status = UsbOtgSuspended;
    /* Upon Resume call usr call back */
//    pdev->dev.usr_cb->DeviceSuspended(); 
}

void UsbDevice::resume()
{
    /* Upon Resume call usr call back */
//    pdev->dev.usr_cb->DeviceResumed(); 
    mDev.dev.device_status = mDev.dev.device_old_status;  
    mDev.dev.device_status = UsbOtgConfigured;
}

void UsbDevice::isoInIncompleteHandler()
{
    if (mCurrentConfig)
        mCurrentConfig->isoInIncomplete();
}

void UsbDevice::isoOutIncompleteHandler()
{
    if (mCurrentConfig)
        mCurrentConfig->isoOutIncomplete();
}
  
#ifdef VBUS_SENSING_ENABLED
void UsbDevice::devConnected()
{
    mDev.dev.connection_status = 1;
}

void UsbDevice::devDisconnected()
{
    if (mCurrentConfig)
        mCurrentConfig->deInit();
    mDev.dev.connection_status = 0; 
}
#endif
//---------------------------------------------------------------------------

//------------------------- SETUP stage handlers ----------------------------

void UsbDevice::handleStdReq(const UsbSetupReq &req)
{
    switch (req.bmRequest & USB_REQ_TYPE_MASK)
    {
      case USB_REQ_TYPE_STANDARD: 
//        printf("std req: 0x%02X\n", req.bRequest);
        switch (req.bRequest)
        {
            case USB_REQ_GET_DESCRIPTOR:    getDescriptor(req); break;
            case USB_REQ_SET_ADDRESS:       setAddress(req);    break;
            case USB_REQ_SET_CONFIGURATION: setConfig(req);     break;
            case USB_REQ_GET_CONFIGURATION: getConfig(req);     break;
            case USB_REQ_GET_STATUS:        getStatus(req);     break;
            case USB_REQ_SET_FEATURE:       setFeature(req);    break;
            case USB_REQ_CLEAR_FEATURE:     clearFeature(req);  break;
            default: ctlError(req);
        }
        break;
        
      case USB_REQ_TYPE_CLASS:
        classSetupRequest(req);
        break;
        
      case USB_REQ_TYPE_VENDOR:
//        printf("vendor req\n");
        vendorSetupRequest(req);
        break;
    }
}

void UsbDevice::handleStdItfReq(const UsbSetupReq &req)
{
    switch (mDev.dev.device_status) 
    {
      case UsbOtgConfigured:
        if ((req.wIndex & 0xFF) <= childrenCount()) 
        {
            if (mCurrentConfig)
                mCurrentConfig->setup(req);
            if (req.wLength == 0)
                mDrv->ctlSendStatus();
        } 
        else 
        {                                               
           ctlError(req);
        }
        break;

      default:
        ctlError(req);
        break;
    }
}

void UsbDevice::handleStdEpReq(const UsbSetupReq &req)
{
    uint8_t ep_addr = req.wIndex & 0xFF;
    unsigned short epStatus = 0;

    switch (req.bRequest) 
    {
      case USB_REQ_SET_FEATURE:
        switch (mDev.dev.device_status) 
        {
          case UsbOtgAddressed:
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
                mDrv->epStall(ep_addr);
            break;	

          case UsbOtgConfigured:
            if (req.wValue == USB_FEATURE_EP_HALT)
            {
                if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
                mDrv->epStall(ep_addr);
            }
            if (mCurrentConfig)
                mCurrentConfig->setup(req);
            mDrv->ctlSendStatus();
            break;

          default:                         
            ctlError(req);  
        }
        break;

      case USB_REQ_CLEAR_FEATURE:
        switch (mDev.dev.device_status) 
        {
          case UsbOtgAddressed:          
            if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
                mDrv->epStall(ep_addr);
            break;	

          case UsbOtgConfigured:
            if (req.wValue == USB_FEATURE_EP_HALT)
            {
                if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
                {        
                    mDrv->epClrStall(ep_addr);
//                    if (mCurrentConfig)
//                        mCurrentConfig->setup(req);
                }
                mDrv->ctlSendStatus();
            }
            break;

          default:         
            ctlError(req);
        }
        break;

      case USB_REQ_GET_STATUS:                  
        switch (mDev.dev.device_status) 
        {
          case UsbOtgAddressed:
            if ((ep_addr != 0x00) && (ep_addr != 0x80)) 
                mDrv->epStall(ep_addr);
            break;	

          case UsbOtgConfigured:
            if ((ep_addr & 0x80) == 0x80)
                epStatus = (mDev.dev.in_ep[ep_addr & 0x7F].is_stall)? 1: 0;
            else if ((ep_addr & 0x80)== 0x00)
                epStatus = (mDev.dev.out_ep[ep_addr].is_stall)? 1: 0;
            mDrv->ctlSendData(&epStatus, 2);
            break;

          default:                         
            ctlError(req);
        }
        break;

      default:
        break;
    }
}
//---------------------------------------------------------------------------

void UsbDevice::getDescriptor(const UsbSetupReq &req)
{
    ByteArray buf;
    unsigned char strno = req.wValue;
    
    switch (req.wValue>>8)
    {
      case USB_DESC_TYPE_DEVICE:
        readDescriptor(buf, false);
        if (req.wLength == 64 || mDev.dev.device_status == UsbOtgDefault) // !!!! wtf?? 4O 3A WAMAHCTBO??!! 3. bl. eto ne ya.
            buf.resize(8);
        break;
        
      case USB_DESC_TYPE_CONFIGURATION:
        defaultConfiguration()->readDescriptor(buf, true);
        // tut 4ot pro other config descriptor, HO poka tak
        // pdev->dev.pConfig_descriptor = buf.data();// tak nelza ibo buf is local var, HO wH9ra BCE PABHO HE usaetsa
        break;
        
      case USB_DESC_TYPE_STRING:
        if (strno < mStrings.size())
            buf = mStrings[strno];
        break;
        
      case USB_DESC_TYPE_DEVICE_QUALIFIER:
        if (mDev.cfg.speed == USB_OTG_SPEED_HIGH)
            buf = mDeviceQualifierDescriptor.toByteArray();
        break;
        
      case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
//        otherConfiguration()->readDescriptor(buf, true); // noKa TakoBou HeT
        break;
    }
    
    if (buf.size() > req.wLength)
        buf.resize(req.wLength);
    
    if (buf.size())
        mDrv->ctlSendData(buf);
    else
        ctlError(req);
}

void UsbDevice::setAddress(const UsbSetupReq &req)
{
    if ((req.wIndex == 0) && (req.wLength == 0))
    {
        unsigned char devAddr = req.wValue & 0x7F;
        if (mDev.dev.device_status == UsbOtgConfigured)
        {
            ctlError(req);
        }
        else
        {
            mDev.dev.device_address = devAddr;
            mDrv->epSetAddress(devAddr);
            mDrv->ctlSendStatus();
            mDev.dev.device_status = devAddr? UsbOtgAddressed: UsbOtgDefault;
        }
    }
    else
    {
        ctlError(req);
    }
}

void UsbDevice::setConfig(const UsbSetupReq &req)
{
    unsigned char cfgidx = req.wValue;
    
    if (cfgidx > childrenCount()) // max configurations
    {            
        ctlError(req);                              
    } 
    else 
    {
        switch (mDev.dev.device_status) 
        {
          case UsbOtgAddressed:
            if (cfgidx) 
            {                                			   							   							   				
                mDev.dev.device_config = cfgidx;
                mDev.dev.device_status = UsbOtgConfigured;
                setConfig(cfgidx);
                mDrv->ctlSendStatus();
            }
            else 
            {
                mDrv->ctlSendStatus();
            }
            break;

          case UsbOtgConfigured:
            if (cfgidx == 0) 
            {                           
                mDev.dev.device_status = UsbOtgAddressed;
                mDev.dev.device_config = cfgidx;          
                clearConfig(cfgidx);
                mDrv->ctlSendStatus();
            } 
            else if (cfgidx != mDev.dev.device_config) 
            {
                clearConfig(mDev.dev.device_config);
                mDev.dev.device_config = cfgidx;
                setConfig(cfgidx);
                mDrv->ctlSendStatus();
            }
            else
            {
                mDrv->ctlSendStatus();
            }
            break;

          default:					
            ctlError(req);                     
            break;
        }
    }
}

void UsbDevice::getConfig(const UsbSetupReq &req)
{
    if (req.wLength != 1) 
    {                   
        ctlError(req);
    }
    else 
    {
        switch (mDev.dev.device_status)  
        {
            case UsbOtgAddressed: mDrv->ctlSendData(&mDefaultConfig, 1); break;
            case UsbOtgConfigured: mDrv->ctlSendData(&mDev.dev.device_config, 1); break;
            default: ctlError(req);
        }
    }
}

void UsbDevice::getStatus(const UsbSetupReq &req)
{
    unsigned int cfgStatus = 0;
    switch (mDev.dev.device_status) 
    {
      case UsbOtgAddressed:
      case UsbOtgConfigured:
        if (mCurrentConfig)
            if (mCurrentConfig->descriptor()->attributes() & AttrSelfPowered)
                cfgStatus |= 1;
        if (mDev.dev.DevRemoteWakeup) 
            cfgStatus |= 2;                                
        mDrv->ctlSendData(&cfgStatus, 2);
        break;

      default:
        ctlError(req);                        
    }
}

void UsbDevice::setFeature(const UsbSetupReq &req)
{
    if (req.wValue == USB_FEATURE_REMOTE_WAKEUP)
    {
        mDev.dev.DevRemoteWakeup = 1;  
        if (mCurrentConfig)
            mCurrentConfig->setup(req);
        mDrv->ctlSendStatus();
    }
    else if ((req.wValue == USB_FEATURE_TEST_MODE) && ((req.wIndex & 0xFF) == 0))
    {
        USB_OTG_DCTL_TypeDef dctl;
        dctl.d32 = USB_OTG_READ_REG32(&mDev.regs.DREGS->DCTL);
        dctl.b.tstctl = req.wIndex >> 8;
        setTestMode(dctl);
        mDev.dev.test_mode = 1;
        mDrv->ctlSendStatus();
    }
}

void UsbDevice::clearFeature(const UsbSetupReq &req)
{
    switch (mDev.dev.device_status)
    {
      case UsbOtgAddressed:
      case UsbOtgConfigured:
        if (req.wValue == USB_FEATURE_REMOTE_WAKEUP) 
        {
            mDev.dev.DevRemoteWakeup = 0;
            if (mCurrentConfig)
                mCurrentConfig->setup(req);
            mDrv->ctlSendStatus();
        }
        break;

      default :
        ctlError(req);
        break;
    }
}
//---------------------------------------------------------------------------

void UsbDevice::ctlError(const UsbSetupReq &req)
{
    if ((req.bmRequest & 0x80) == 0x80)
        mDrv->ctlError(0x80);
    else if (req.wLength == 0)
        mDrv->ctlError(0x80);
    else
        mDrv->ctlError(0);
}
//---------------------------------------------------------------------------

void UsbDevice::setConfig(unsigned char cfgidx)
{
    mConfigIdx = cfgidx;
    UsbNode *cfg = getChildByNumber(cfgidx);
    if (cfg)
    {
        mCurrentConfig = dynamic_cast<UsbConfiguration*>(cfg);
        cfg->init(); // recursively init the branch till endpoints
    }
}

void UsbDevice::clearConfig(unsigned char cfgidx)
{
    mConfigIdx = 0;
    mCurrentConfig = 0L;
    UsbNode *cfg = getChildByNumber(cfgidx);
    if (cfg)
        cfg->deInit(); // recursively deInit the branch till endpoints
    mEndpoints.clear();
}
//---------------------------------------------------------------------------

unsigned char UsbDevice::addStringDescriptor(const string &s)
{
    mStrings.push_back(StringDescriptor(s).toByteArray());
    return mStrings.size() - 1;
}

unsigned char UsbDevice::addStringDescriptor(const wstring &s)
{
    mStrings.push_back(StringDescriptor(s).toByteArray());
    return mStrings.size() - 1;
}
//---------------------------------------------------------------------------

void UsbDevice::connectEndpoint(UsbEndpoint *ep)
{
    mEndpoints[ep->number()] = ep;
}

void UsbDevice::disconnectEndpoint(UsbEndpoint *ep)
{
    mEndpoints.erase(ep->number());
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void UsbDevice::bspInit()
{
    if (mUsbCore == OtgFs)
    {
        Gpio::config(2, Gpio::OTG_FS_DM, Gpio::OTG_FS_DP);//,  Gpio::OTG_FS_SOF);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
        RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE); 
    }
    else if (mUsbCore == OtgHs)
    {
        Gpio::config(4, Gpio::OTG_HS_DM, Gpio::OTG_HS_DP, Gpio::OTG_HS_ID, Gpio::OTG_HS_VBUS);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_OTG_HS, ENABLE) ;  
    }
    else if (mUsbCore == OtgHsUlpi)
    {
        Gpio::config(8, Gpio::OTG_HS_ULPI_D0, Gpio::OTG_HS_ULPI_D1, Gpio::OTG_HS_ULPI_D2, Gpio::OTG_HS_ULPI_D3, 
                        Gpio::OTG_HS_ULPI_D4, Gpio::OTG_HS_ULPI_D5, Gpio::OTG_HS_ULPI_D6, Gpio::OTG_HS_ULPI_D7);
        Gpio::config(4, Gpio::OTG_HS_ULPI_CLK, Gpio::OTG_HS_ULPI_NXT, Gpio::OTG_HS_ULPI_DIR, Gpio::OTG_HS_ULPI_STP);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_OTG_HS | RCC_AHB1Periph_OTG_HS_ULPI, ENABLE) ; 
    }
    //RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE); // x3 otsuda ili net
}

void UsbDevice::bspEnableInterrupt()
{
    NVIC_InitTypeDef NVIC_InitStructure; 
  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    if (mUsbCore == OtgFs)
        NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
    else
        NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
    
    #ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
        NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_OUT_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);  

        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
        NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_IN_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);   
    #endif  
}
//---------------------------------------------------------------------------

void UsbDevice::stop()
{
    USB_OTG_StopDevice(&mDev);
    mDev.regs.GREGS->GCCFG = 0;
}

//------------------------------ interrupts ---------------------------------
#ifdef __cplusplus
 extern "C" {
#endif 

void OTG_FS_IRQHandler(void)
{
    UsbDevice *dev = UsbDevice::fsCore();
    if (!dev)
        return;
    
    dev->driver()->isrHandler();
}

void OTG_HS_IRQHandler(void)
{
    UsbDevice *dev = UsbDevice::hsCore();
    if (!dev)
        return;
    
    dev->driver()->isrHandler();
}
   
#ifdef __cplusplus
}
#endif