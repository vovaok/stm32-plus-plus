#if defined(STM32G4)

#include "fdcan.h"
#include "rcc.h"

FdCan *FdCan::m_instances[3] = {nullptr, nullptr, nullptr};

FdCan::FdCan(Gpio::Config fdcanRx, Gpio::Config fdcanTx) :
    CanInterface(2)
{
    if (GpioConfigGetPeriphNumber(fdcanRx) !=
        GpioConfigGetPeriphNumber(fdcanTx))
        THROW(Exception::InvalidPin);

    Gpio::config(fdcanRx);
    Gpio::config(fdcanTx);

    switch (GpioConfigGetPeriphNumber(fdcanRx))
    {
        case 1: m_dev = FDCAN1; msgRam = (MessageRAM*)(SRAMCAN_BASE); break;
        case 2: m_dev = FDCAN2; msgRam = (MessageRAM*)(SRAMCAN_BASE + sizeof(MessageRAM)); break;
        case 3: m_dev = FDCAN3; msgRam = (MessageRAM*)(SRAMCAN_BASE + 2*sizeof(MessageRAM)); break;
        default: THROW(Exception::InvalidPeriph);
    }

    RCC->CCIPR |= RCC_CCIPR_FDCANSEL_1; // select PCLK as clock source
    rcc().setPeriphEnabled(m_dev);

    memset(msgRam, 0, sizeof(MessageRAM));

    close();

    setBaudrate(1000000);

//    open();
}

int FdCan::configureFilter(Flags flags, uint32_t id, uint32_t mask, int fifoChannel)
{
    int idx = -1;
    if (flags & ExtId)
    {
        // find free filter
        for (idx=0; idx<8; idx++)
        {
            if (!msgRam->extFilters[idx].EFEC)
                break;
        }
        if (idx >= 8)
            return -1;

        ExtFilterElement filt;
        filt.EFID1 = id;
        filt.EFID2 = mask;
        filt.EFT = 0x2; // classic filter
        filt.EFEC = fifoChannel + 1;
        msgRam->extFilters[idx] = filt;
        return idx;
    }
    else
    {
        // find free filter
        for (idx=0; idx<28; idx++)
        {
            if (!msgRam->stdFilters[idx].SFEC)
                break;
        }
        if (idx >= 28)
            return -1;

        StdFilterElement filt;
        filt.SFID1 = id;
        filt.SFID2 = mask;
        filt.SFT = 0x2; // classic filter
        filt.SFEC = fifoChannel + 1;
        msgRam->stdFilters[idx] = filt;
        return 0x80 | idx;
    }
}

bool FdCan::removeFilter(int index)
{
    if (index & 0x80) // std filter
    {
        index &= ~0x80;
        if (index < 28)
        {
            msgRam->stdFilters[index] = StdFilterElement();
            return true;
        }
    }
    else
    {
        if (index < 8)
        {
            msgRam->extFilters[index] = ExtFilterElement();
            return true;
        }
    }
    return false;
}

int FdCan::pendingMessageLength(int fifoChannel)
{
    RxFifoElement *elem = nextRxMessage(fifoChannel);
    if (elem)
        return sizeFromDLC(elem->DLC);
    return -1;
}

int FdCan::pendingMessageFilterIdx(int fifoChannel)
{
    RxFifoElement *elem = nextRxMessage(fifoChannel);
    if (elem)
        return elem->FIDX;
    return -1;
}

int FdCan::receiveMessage(uint32_t *id, uint8_t *data, uint8_t maxsize, int fifoChannel)
{
    RxFifoElement *elem = nextRxMessage(fifoChannel);
    if (elem)
    {
        if (elem->XTD)
            *id = elem->ID;
        else
            *id = elem->ID >> 18;
        int size = sizeFromDLC(elem->DLC);
        if (size > maxsize)
            size = maxsize;

        uint8_t *src = reinterpret_cast<uint8_t *>(elem->data);
        int cnt = size;
        while (cnt--)
            *data++ = *src++;

        // release the FIFO
        if (elem < msgRam->rxFifo1)
            m_dev->RXF0A = (m_dev->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
        else
            m_dev->RXF1A = (m_dev->RXF1S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;

        return size;
    }
    return -1;
}

FdCan::RxFifoElement *FdCan::nextRxMessage(int fifoChannel)
{
    if (fifoChannel == 0)
    {
        if (m_dev->RXF0S & FDCAN_RXF0S_F0FL)
        {
            int idx = (m_dev->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
            return msgRam->rxFifo0 + idx;
        }
    }
    else
    {
        if (m_dev->RXF1S & FDCAN_RXF1S_F1FL)
        {
            int idx = (m_dev->RXF1S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
            return msgRam->rxFifo1 + idx;
        }
    }
    return nullptr;
}

bool FdCan::transmitMessage(Flags flags, uint32_t id, const uint8_t *data, uint8_t size)
{
//    if (!(m_dev->TXFQS & FDCAN_TXFQS_TFFL))
//        return false;
//    if ((m_dev->TXBRP & FDCAN_TXBRP_TRP) == FDCAN_TXBRP_TRP)
//        return false;
    if (m_dev->TXFQS & FDCAN_TXFQS_TFQF) // FIFO is full
        return false;

    // retrieve index of next free element
    int idx = (m_dev->TXFQS & FDCAN_TXFQS_TFQPI) >> FDCAN_TXFQS_TFQPI_Pos;

    TxBufferHeader hdr;
    if (flags & ExtId)
    {
        hdr.XTD = 1;
        hdr.ID = id;
    }
    else
        hdr.ID = id << 18;

    if (flags & FD)
        hdr.FDF = 1;
    if (flags & BRS)
        hdr.BRS = 1;
    hdr.DLC = calcDLC(size);


    const uint32_t *src = reinterpret_cast<const uint32_t *>(data);
    uint32_t *dst = msgRam->txBuffers[idx].words;
    *dst++ = hdr.words[0];
    *dst++ = hdr.words[1];
    size = (size + 3) / 4;
    while (size--)
        *dst++ = *src++;

    // activate transmission reauest
    m_dev->TXBAR = 1 << idx;
    return true;
}

bool FdCan::setBaudrate(int value)
{
    // if (isOpen())
    //     return false;

    int clkin = rcc().getPeriphClk(m_dev);
    int psc = 0;
    int mod = 0;
    int btq = 0;
    do
    {
        psc++;
        btq = (clkin / psc) / value;
        mod = (clkin / psc) % value;
        if (psc > 512)
            return false;
    }
    while (btq > 320 || mod); // prescaler choosed assumpting max bit length is 320tq

    --psc;
    // assume T1 / T2 = 4
    int t2 = (btq + 2) / 5 - 1; // length of T2 segment - 1
    int t1 = btq - t2 - 3; // length of T1 segment - 1
    int sjw = t2; // sync jump width

    m_dev->NBTP = (t1 << FDCAN_NBTP_NTSEG1_Pos) | (t2 << FDCAN_NBTP_NTSEG2_Pos)
                | (psc << FDCAN_NBTP_NBRP_Pos) | (sjw << FDCAN_NBTP_NSJW_Pos);
    return true;
}

bool FdCan::setDataBaudrate(int value)
{
    int clkin = rcc().getPeriphClk(m_dev);
    int psc = 0;
    int mod = 0;
    int btq = 0;
    do
    {
        psc++;
        btq = (clkin / psc) / value;
        mod = (clkin / psc) % value;
        if (psc > 32)
            return false;
    }
    while (btq > 40 || mod); // prescaler choosed assumpting max bit length is 40tq

    --psc;
    // assume T1 / T2 = 4
    int t2 = (btq + 2) / 5 - 1; // length of T2 segment - 1
    int t1 = btq - t2 - 3; // length of T1 segment - 1
    int sjw = t2; // sync jump width

    m_dev->DBTP = (t1 << FDCAN_DBTP_DTSEG1_Pos) | (t2 << FDCAN_DBTP_DTSEG2_Pos)
                | (psc << FDCAN_DBTP_DBRP_Pos) | (sjw << FDCAN_DBTP_DSJW_Pos);

    //! @todo check the update of BRSE and FDOE in the CCCR
    m_dev->CCCR |= FDCAN_CCCR_BRSE | FDCAN_CCCR_FDOE;

    return true;
}

bool FdCan::open(Device::OpenMode mode)
{
    // if already opened...
    if (!(m_dev->CCCR & FDCAN_CCCR_INIT))
        return false;

    // choose Bus monitoring mode if no write allowed
    if (mode & Device::WriteOnly)
        m_dev->CCCR &= ~FDCAN_CCCR_MON;
    else
        m_dev->CCCR |= FDCAN_CCCR_MON;

    // enter Normal mode
    m_dev->CCCR &= ~FDCAN_CCCR_INIT;
    while (m_dev->CCCR & FDCAN_CCCR_INIT);
    return true;
}

bool FdCan::close()
{
    // request Initialization mode
    m_dev->CCCR |= FDCAN_CCCR_INIT;
    while (!(m_dev->CCCR & FDCAN_CCCR_INIT));
    // enable configuration
    m_dev->CCCR |= FDCAN_CCCR_CCE;
    return true;
}

bool FdCan::isOpen() const
{
    return !(m_dev->CCCR & FDCAN_CCCR_INIT);
}

void FdCan::setRxInterruptEnabled(int fifoChannel, bool enabled)
{
    //! @todo todo todotodotodto

//    IRQn_Type IRQn;
//    uint32_t mask;
//    if (fifoChannel == 0)
//    {
//        IRQn = (m_dev == CAN1)? CAN1_RX0_IRQn: CAN2_RX0_IRQn;
//        mask = CAN_IER_FMPIE0;// CAN_IER_FFIE0;
//    }
//    else
//    {
//        IRQn = (m_can == CAN1)? CAN1_RX1_IRQn: CAN2_RX1_IRQn;
//        mask = CAN_IER_FMPIE1;// CAN_IER_FFIE1;
//    }
//
//    NVIC_SetPriority(IRQn, 3);
//    NVIC_EnableIRQ(IRQn);
//
//    if (enabled)
//        m_can->IER |= mask;
//    else
//        m_can->IER &= ~mask;
}

int FdCan::calcDLC(int size)
{
    if (size <= 8)
        return size;
    else if (size <= 12)
        return 9;
    else if (size <= 16)
        return 10;
    else if (size <= 20)
        return 11;
    else if (size <= 24)
        return 12;
    else if (size <= 32)
        return 13;
    else if (size <= 48)
        return 14;
    return 15;
}

int FdCan::sizeFromDLC(uint8_t DLC)
{
    static const uint8_t DLCtoBytes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
    return DLCtoBytes[DLC];
}

extern "C"
{

void FDCAN1_IT0_IRQHandler()
{

}

void FDCAN1_IT1_IRQHandler()
{

}

void FDCAN2_IT0_IRQHandler()
{

}

void FDCAN2_IT1_IRQHandler()
{

}

void FDCAN3_IT0_IRQHandler()
{

}

void FDCAN3_IT1_IRQHandler()
{

}

}

#endif