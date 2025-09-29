#include "can.h"
#include "rcc.h"

Can *Can::m_instances[2] = {nullptr, nullptr};

Can::Can(Gpio::Config pinRx, Gpio::Config pinTx, int baudrate) :
    CanInterface(2)
{
    int canNumber = GpioConfigGetPeriphNumber(pinRx);
    if (canNumber == 0)
        THROW(Exception::InvalidPin);
    if (pinRx != Gpio::NoConfig && canNumber != GpioConfigGetPeriphNumber(pinTx))
        THROW(Exception::InvalidPin);

    switch (canNumber)
    {
        case 1: m_can = CAN1; break;
        case 2: m_can = CAN2; break;
        default: THROW(Exception::InvalidPeriph);
    }

    m_instances[canNumber-1] = this;

    // CAN2 take SRAM access through CAN1 memory bus, so it must be enabled always
    rcc().setPeriphEnabled(CAN1);
    if (m_can == CAN2)
        rcc().setPeriphEnabled(CAN2);

    Gpio::config(pinRx);
    Gpio::config(pinTx);

    /// @todo Maybe implement timeout check

    // request Initialization mode
    close();
//    m_can->MCR |= CAN_MCR_INRQ;
//    while (!(m_can->MSR & CAN_MSR_INAK));

    // exit sleep mode
    m_can->MCR &= ~CAN_MCR_SLEEP;
    while (m_can->MSR & CAN_MSR_SLAK);

    // these bits should be 0 after reset
//    m_can->MCR &= ~(CAN_MCR_TTCM | CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_NART | CAN_MCR_RFLM | CAN_MCR_TXFP);
    m_can->MCR |= CAN_MCR_ABOM | CAN_MCR_TXFP;

    int clkin = rcc().getPeriphClk(m_can);
    int psc = 0;
    int mod = 0;
    int btq = 0;
    do
    {
        psc++;
        btq = (clkin / psc) / (baudrate);
        mod = (clkin / psc) % (baudrate);
        if (psc >= 1024)
            THROW(Exception::OutOfRange);
    }
    while (btq > 25 || mod); // prescaler choosed assumpting max bit length is 25tq

    int t2 = btq<=17? ((btq-1)>>1): 8; // length of T2 segment
    int t1 = btq - 1 - t2; // length of T1 segment

    --t1; --t2;
    // sync jump width = 1tq
    m_can->BTR = (t1 << CAN_BTR_TS1_Pos) | (t2 << CAN_BTR_TS2_Pos) | (psc - 1);

    if (m_can == CAN2)
    {
        m_firstFilterIdx = 14;
        // in theory, this is already done after reset:
        CAN1->FMR = m_firstFilterIdx << 8; // allocate 14 filters for CAN2 core
    }

    open();
}
//---------------------------------------------------------------------------

bool Can::open(Device::OpenMode mode)
{
    // if already opened...
    if (!(m_can->MSR & CAN_MSR_INAK))
        return false;

    // choose Silent mode if no write allowed
    if (mode & Device::WriteOnly)
        m_can->BTR &= ~CAN_BTR_SILM;
    else
        m_can->BTR |= CAN_BTR_SILM;

    // enter Normal mode
    m_can->MCR &= ~CAN_MCR_INRQ;
    while (m_can->MSR & CAN_MSR_INAK);
    return true;
}

bool Can::close()
{
    // request Initialization mode
    m_can->MCR |= CAN_MCR_INRQ;
    while (!(m_can->MSR & CAN_MSR_INAK));
    return true;
}

bool Can::isOpen() const
{
    return !(m_can->MSR & CAN_MSR_INAK);
}

int Can::configureFilter(Flags flags, uint32_t id, uint32_t mask, int fifoChannel)
{
    // this function configures filter in 32-bit Identifier Mask mode
    CAN_TypeDef *can = CAN1;

    if (fifoChannel < 0 || fifoChannel > 1)
        return -1; // ERROR: invalid FIFO channel provided

    // find free filter
    uint32_t filterBit = 1 << m_firstFilterIdx;
    int idx;
    for (idx=0; idx<14; idx++, filterBit<<=1)
    {
        if (!(can->FA1R & filterBit))
            break;
    }

    // if no more filters available:
    if (idx == 14)
        return -1;

    idx += m_firstFilterIdx;

    // enter filter initialization mode
    can->FMR |= CAN_FMR_FINIT;
    // select Identifier Mask mode
    can->FM1R &= ~filterBit;
    // select Single 32-bit scale configuration
    can->FS1R |= filterBit;
    // select FIFO channel
    if (fifoChannel)
        can->FFA1R |= filterBit;
    else
        can->FFA1R &= ~filterBit;

    // set ID and mask
    if (flags & ExtId)
    {
        id = (id << CAN_RI0R_EXID_Pos) | (1 << 2);
        mask = (mask << CAN_RI0R_EXID_Pos) | (1 << 2);
    }
    else
    {
        id = (id << CAN_RI0R_STID_Pos) | (0 << 2);
        mask = (mask << CAN_RI0R_STID_Pos) | (1 << 2);
    }

    can->sFilterRegister[idx].FR1 = id;
    can->sFilterRegister[idx].FR2 = mask;
    // activate the filter
    can->FA1R |= filterBit;

    // exit filter initialization mode
    can->FMR &= ~CAN_FMR_FINIT;

    // don't enable interrupt!
    // it will be done on the socket activation!
//    setRxInterruptEnabled(fifoChannel, true);

    return idx - m_firstFilterIdx;
}

bool Can::removeFilter(int index)
{
    if (index < 0 || index >= 14)
        return false;

    CAN_TypeDef *can = CAN1;

    can->FMR |= CAN_FMR_FINIT;
    can->FA1R &= ~(1 << (index + m_firstFilterIdx));
    can->FMR &= ~CAN_FMR_FINIT;
    return true;
}

void Can::setRxInterruptEnabled(int fifoChannel, bool enabled)
{
    IRQn_Type IRQn;
    uint32_t mask;
    if (fifoChannel == 0)
    {
        IRQn = (m_can == CAN1)? CAN1_RX0_IRQn: CAN2_RX0_IRQn;
        mask = CAN_IER_FMPIE0;// CAN_IER_FFIE0;
    }
    else
    {
        IRQn = (m_can == CAN1)? CAN1_RX1_IRQn: CAN2_RX1_IRQn;
        mask = CAN_IER_FMPIE1;// CAN_IER_FFIE1;
    }

    NVIC_SetPriority(IRQn, 3);
    NVIC_EnableIRQ(IRQn);

    if (enabled)
        m_can->IER |= mask;
    else
        m_can->IER &= ~mask;
}
//---------------------------------------------------------------------------

int Can::pendingMessageLength(int fifoChannel)
{
    if (!isRxMessagePending(fifoChannel))
        return -1;
    return m_can->sFIFOMailBox[fifoChannel].RDTR & CAN_RDT0R_DLC;
}

int Can::pendingMessageFilterIdx(int fifoChannel)
{
    if (!isRxMessagePending(fifoChannel))
        return -1;
    int fmi = (m_can->sFIFOMailBox[fifoChannel].RDTR & CAN_RDT0R_FMI) >> CAN_RDT0R_FMI_Pos;
    return (fmi << 1) + fifoChannel;
}

int Can::receiveMessage(uint32_t *id, uint8_t *data, uint8_t maxsize, int fifoChannel)
{
    if (!isRxMessagePending(fifoChannel))
        return -1;

    CAN_FIFOMailBox_TypeDef *fifo = m_can->sFIFOMailBox + fifoChannel;

    // read data from the FIFO
    bool ext_id = fifo->RIR & CAN_RI0R_IDE;
    if (ext_id)
        *id = fifo->RIR >> CAN_RI0R_EXID_Pos;
    else
        *id = fifo->RIR >> CAN_RI0R_STID_Pos;
    int size = fifo->RDTR & CAN_RDT0R_DLC;
    if (size > maxsize)
        size = maxsize;

    uint32_t w[2];
    w[0] = fifo->RDLR;
    w[1] = fifo->RDHR;
    const uint8_t *src = reinterpret_cast<const uint8_t *>(w);
    int cnt = size;
    while (cnt--)
        *data++ = *src++;

    // release the FIFO
    if (fifoChannel == 0)
        m_can->RF0R |= CAN_RF0R_RFOM0;
    else if (fifoChannel == 1)
        m_can->RF1R |= CAN_RF1R_RFOM1;

    return size;
}

bool Can::isRxMessagePending(int fifoChannel)
{
    if (fifoChannel == 0 && (m_can->RF0R & CAN_RF0R_FMP0))
        return true;
    if (fifoChannel == 1 && (m_can->RF1R & CAN_RF1R_FMP1))
        return true;
    return false;
}

bool Can::transmitMessage(Flags flags, uint32_t id, const uint8_t *data, uint8_t size)
{
    // check maximum size
    if (size > 8)
        return false;

    uint32_t tsr = m_can->TSR;
    // check free transmit mailboxes
    if (!(tsr & CAN_TSR_TME))
        return false;

    // get index of the next free mailbox
    int idx = (tsr & CAN_TSR_CODE) >> CAN_TSR_CODE_Pos;
    CAN_TxMailBox_TypeDef *mb = m_can->sTxMailBox + idx;

    uint32_t rtr = 0;
    if (flags & RTR)
        rtr = CAN_TI0R_RTR;

    // fill the mailbox
    if (flags & ExtId)
        mb->TIR = (id << CAN_TI0R_EXID_Pos) | CAN_TI0R_IDE | rtr;
    else
        mb->TIR = (id << CAN_TI0R_STID_Pos) | rtr;
    mb->TDTR = size;
    mb->TDLR = reinterpret_cast<const uint32_t*>(data)[0];
    mb->TDHR = reinterpret_cast<const uint32_t*>(data)[1];

    // request transmission;
    mb->TIR |= CAN_TI0R_TXRQ;
    return true;
}
//------------------------- interrupt handlers ------------------------------

#ifdef __cplusplus
 extern "C" {
#endif

void CAN1_RX0_IRQHandler()
{
    CAN1->RF0R = CAN_RF0R_FULL0;
    Can::m_instances[0]->messageReceived(0);
}

void CAN1_RX1_IRQHandler()
{
    CAN1->RF1R = CAN_RF1R_FULL1;
    Can::m_instances[0]->messageReceived(1);
}

void CAN1_TX_IRQHandler()
{
    CAN1->TSR = CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2;
//    Can *can = Can::instance(1);
//    if (can)
//    {
//        can->transmitReadyEvent()();
//    }
}

void CAN2_RX0_IRQHandler()
{
    CAN2->RF0R = CAN_RF0R_FULL0;
    Can::m_instances[1]->messageReceived(0);
}

void CAN2_RX1_IRQHandler()
{
    CAN2->RF1R = CAN_RF1R_FULL1;
    Can::m_instances[1]->messageReceived(1);
}

void CAN2_TX_IRQHandler()
{
    CAN2->TSR = CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2;
//    Can *can = Can::instance(2);
//    if (can)
//    {
//        can->transmitReadyEvent()();
//    }
}

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
