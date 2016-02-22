#include "can_old.h"

OldCan::OldCan(int canNumber, int baudrate, Gpio::Config pinRx, Gpio::Config pinTx) :
  Can(canNumber, baudrate, pinRx, pinTx),
  canAddr(0x01)
{
    addFilter(0, 0);
}
//---------------------------------------------------------------------------

void OldCan::setParameter(unsigned char address, unsigned char group, unsigned char cmd, void *data, unsigned char size)
{
    ObjMessage msg;
    msg.id.cmd = cmd;
    msg.id.group = group;
    msg.id.address = address;
    msg.id.response = 0;
    msg.id.version = OBJNET_PROTOCOL_VERSION;
    msg.id.full = OBJNET_ID_FULL;
    msg.id.not_global = 1;
//    msg.data = data;
//    msg.size = size;
    
    
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.IDE = CAN_ID_EXT;
    TxMessage.ExtId = msg.id.dword;
    TxMessage.DLC = size;
    for(char i=0;i<size;i++)
        TxMessage.Data[i] = ((unsigned char*)data)[i];    
    
    send(TxMessage);
}

bool OldCan::getResponse(ObjMessage &msg)
{
    CanRxMsg rxMsg;
    if (receive(0, rxMsg))
    {
        msg.id.dword = rxMsg.ExtId;
        msg.size = rxMsg.DLC;
        for (char i=0; i<msg.size; i++)
            reinterpret_cast<unsigned char*>(msg.data)[i] = rxMsg.Data[i];
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

