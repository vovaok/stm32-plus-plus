#include "canonbinterface.h"

using namespace Objnet;

CanOnbInterface::CanOnbInterface(CanInterface *can)
{ 
    m_can = new CanSocket(can, CanInterface::ExtId);
    mMaxFrameSize = 8; // 64 if CAN FD
    setTxQueueSize(128);
    mBusType = BusCan;
    m_can->onReadyRead = EVENT(&CanOnbInterface::receiveHandler);
//    mCan->setTransmitReadyEvent(EVENT(&CanOnbInterface::transmitHandler));
    m_can->open();
    can->open();
}
//---------------------------------------------------------------------------

void CanOnbInterface::receiveHandler()
{   
//    CommonMessage msg(m_can->readAll());
    
//    msg.setId(canmsg.ExtId);
//    ByteArray ba(canmsg.Data, canmsg.DLC);
//    msg.setData(std::move(ba));
//    receive(std::move(msg));
    
    receive(CommonMessage(m_can->readAll()));
}

void CanOnbInterface::setReceiveEnabled(bool enabled)
{
    m_can->interface()->setRxInterruptEnabled(m_can->fifoChannel(), enabled);
}

//void CanOnbInterface::transmitHandler()
//{
//    CanTxMsg msg;
//    if (readTx(msg))
//        mCan->send(msg);
//}
//---------------------------------------------------------------------------

bool CanOnbInterface::read(CommonMessage &msg)
{
    ByteArray ba(m_can->read(mMaxFrameSize + 4));
    if (ba.size())
        receive(CommonMessage(ba));
      
    return ObjnetInterface::read(msg);
}

bool CanOnbInterface::send(const CommonMessage &msg)
{
    if (msg.data().size() > mMaxFrameSize)
        return false;
    
    ByteArray ba(4 + msg.size(), '\0');
    ba.resize(4);
    *reinterpret_cast<uint32_t *>(ba.data()) = msg.rawId();
    ba.append(msg.data());
    
    return m_can->write(ba) == ba.size();
}
//---------------------------------------------------------------------------

void CanOnbInterface::flush()
{
    ObjnetInterface::flush();
//    m_can->flush(); 
}
//---------------------------------------------------------------------------

int CanOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    return m_can->addFilter(id, mask);
}

void CanOnbInterface::removeFilter(int number)
{
    m_can->removeFilter(number);
}
//---------------------------------------------------------------------------
