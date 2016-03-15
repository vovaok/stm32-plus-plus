#include "can.h"

//#define CAN_RX_PIN                 GPIO_Pin_0
//#define CAN_TX_PIN                 GPIO_Pin_1
//#define CAN_GPIO_PORT              GPIOD
//#define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOD
//#define CAN_AF_PORT                GPIO_AF_CAN1
//#define CAN_RX_SOURCE              GPIO_PinSource0
//#define CAN_TX_SOURCE              GPIO_PinSource1  

Can *Can::mInstances[2] = {0L, 0L};

Can::Can(int canNumber, int baudrate, Gpio::Config pinRx, Gpio::Config pinTx) :
    mStartFilter(canNumber==2? 14: 0),
    mFilterUsed(0),
    mPacketsReceived(0),
    mPacketsSent(0),
    mPacketsSendFailed(0)
{
    switch (canNumber)
    {
      case 1: 
        mCan = CAN1;
        mInstances[0] = this;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
        break;
        
      case 2:
        mCan = CAN2;
        mInstances[1] = this;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // CAN2 take SRAM access through CAN1 memory bus
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
        break;
        
      default:
        throw Exception::invalidPeriph;
    }
    
    // GPIO pin initialize
    if (pinRx == Gpio::NoConfig)
        pinRx = (canNumber==2)? Gpio::CAN2_RX_PB5: Gpio::CAN1_RX_PD0;
    if (pinTx == Gpio::NoConfig)
        pinTx = (canNumber==2)? Gpio::CAN2_TX_PB6: Gpio::CAN1_TX_PD1;
    Gpio::config(pinRx);
    Gpio::config(pinTx);
    
    /* CAN register init */
    CAN_DeInit(mCan);
    CAN_InitTypeDef CAN_InitStructure; 
    CAN_StructInit(&CAN_InitStructure);
    
    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    
    int APB1freq = Rcc::pClk1();
    int psc = (APB1freq / ((8+16+1)*baudrate)) + 1; // prescaler choosed assumpting max bit length is 25tq
    int btq = APB1freq / (psc*baudrate); // the number of quanta per bit 
    int t2 = btq<=17? ((btq-1)>>1): 8; // length of T2 segment
    int t1 = btq - 1 - t2; // length of T1 segment
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = t1 - 1;
    CAN_InitStructure.CAN_BS2 = t2 - 1;
    CAN_InitStructure.CAN_Prescaler = psc;
    CAN_Init(mCan, &CAN_InitStructure);
      
    /* Enable FIFO 0 message pending Interrupt */
    // CAN_ITConfig(CANx, CAN_IT_FMP0, DISABLE);
}

Can::~Can()
{
    for (int i=0; i<sizeof(mInstances)/sizeof(mInstances[0]); i++)
    {
        if (this == mInstances[i])
        {
            mInstances[i] = 0L;
            return;
        }
    }
}
//---------------------------------------------------------------------------

int Can::addFilter(unsigned long id, unsigned long mask, int fifoNumber)
{
    int filter = mStartFilter;
    // find free filter
    for (int i=0; i<14; i++)
    {
        if (!(mFilterUsed & (1<<i)))
        {
            filter += i;
            mFilterUsed |= (1<<i);
            break;
        }
    }
  
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    CAN_FilterInitStructure.CAN_FilterNumber = filter;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = id >> 13;
    CAN_FilterInitStructure.CAN_FilterIdLow = (id<<3) & 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = mask >> 13;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = (mask<<3) & 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = fifoNumber;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    return filter;
}

void Can::removeFilter(int number)
{
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    CAN_FilterInitStructure.CAN_FilterNumber = number;
    CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    int idx = number - mStartFilter; 
    mFilterUsed &= ~(1<<idx);
}
//---------------------------------------------------------------------------

bool Can::send(CanTxMsg &msg)
{
    msg.RTR = CAN_RTR_DATA;
    msg.IDE = CAN_ID_EXT;
    if (CAN_Transmit(mCan, &msg) == CAN_TxStatus_NoMailBox)
    {
        mPacketsSendFailed++;
        return false;
    }
    mPacketsSent++;
    return true;
}

bool Can::receive(unsigned char fifoNumber, CanRxMsg &msg)
{
    if (fifoNumber > 1)
        return false;
    if ((CAN_MessagePending(mCan, fifoNumber)) > 0)
    {
        CAN_Receive(mCan, fifoNumber, &msg);
        mPacketsReceived++;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

void Can::flush()
{
    CAN_CancelTransmit(mCan, 0);
    CAN_CancelTransmit(mCan, 1);
    CAN_CancelTransmit(mCan, 2);
}

void Can::free(unsigned char fifoNumber)
{
    CAN_FIFORelease(mCan, fifoNumber);
}
//---------------------------------------------------------------------------

void Can::setReceiveEvent(CanReceiveEvent event)
{
    mReceiveEvent = event;
    
    CAN_ITConfig(mCan, CAN_IT_FF0 | CAN_IT_FF1, ENABLE);
    
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = (mCan==CAN1)? CAN1_RX0_IRQn: (mCan==CAN2)? CAN2_RX0_IRQn: 0;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    nvic.NVIC_IRQChannel = (mCan==CAN1)? CAN1_RX1_IRQn: (mCan==CAN2)? CAN2_RX1_IRQn: 0;
    NVIC_Init(&nvic);
}

void Can::setTransmitReadyEvent(NotifyEvent event)
{
    mTransmitReadyEvent = event;
    
    // CAN_RX0_interrupt_enable()
    CAN_ITConfig(mCan, CAN_IT_TME, ENABLE);
    
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = (mCan==CAN1)? CAN1_TX_IRQn: (mCan==CAN2)? CAN2_TX_IRQn: 0;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}
//------------------------- interrupt handlers ------------------------------

#ifdef __cplusplus
 extern "C" {
#endif 

void CAN1_RX0_IRQHandler()
{
    CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);  
  
    Can *can = Can::instance(1);
    if (can)
    {
        CanRxMsg msg;
        while (can->receive(0, msg))
            can->receiveEvent()(0, msg);
    }
}

void CAN1_RX1_IRQHandler()
{
    CAN_ClearITPendingBit(CAN1, CAN_IT_FF1);  
  
    Can *can = Can::instance(1);
    if (can)
    {
        CanRxMsg msg;
        while (can->receive(1, msg))
            can->receiveEvent()(1, msg);
    }
}

void CAN1_TX_IRQHandler()
{
    CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
    Can *can = Can::instance(1);
    if (can)
    {
        can->transmitReadyEvent()();
    }
}

void CAN2_RX0_IRQHandler()
{
    CAN_ClearITPendingBit(CAN2, CAN_IT_FF0);  
  
    Can *can = Can::instance(2);
    if (can)
    {
        CanRxMsg msg;
        while (can->receive(0, msg))
            can->receiveEvent()(0, msg);
    }
}

void CAN2_RX1_IRQHandler()
{
    CAN_ClearITPendingBit(CAN2, CAN_IT_FF1);  
  
    Can *can = Can::instance(2);
    if (can)
    {
        CanRxMsg msg;
        while (can->receive(1, msg))
            can->receiveEvent()(1, msg);
    }
}

void CAN2_TX_IRQHandler()
{
    CAN_ClearITPendingBit(CAN2, CAN_IT_TME);
    Can *can = Can::instance(2);
    if (can)
    {
        can->transmitReadyEvent()();
    }
}

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
