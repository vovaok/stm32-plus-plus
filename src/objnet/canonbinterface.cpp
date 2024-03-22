#include "canonbinterface.h"

using namespace Objnet;

CanOnbInterface::CanOnbInterface(Can *can, int fifoNumber) :
    mCan(can),
    mCurFilterFifo(fifoNumber)
{ 
    mMaxFrameSize = 8;
    setTxQueueSize(128);
    mBusType = BusCan;
    mCan->setReceiveEvent(EVENT(&CanOnbInterface::receiveHandler));
//    mCan->setTransmitReadyEvent(EVENT(&CanOnbInterface::transmitHandler));
}
//---------------------------------------------------------------------------

//bool CanOnbInterface::readRx(CanRxMsg &msg)
//{
//    bool result = false;
//    
//    mCan->setRxInterruptEnabled(false);
//    
//    if (!mRxQueue.empty())
//    {
//        msg = mRxQueue.front();
//        mRxQueue.pop();
//        result = true;
//    }
//    else if (mCan->receive(0, msg))
//    {
//        result = true;
//    }
//    else if (mCan->receive(1, msg))
//    {
//        result = true; 
//    }
//    
//    mCan->setRxInterruptEnabled(true);   
//    
//    return result;
//}
//
//bool CanOnbInterface::writeRx(CanRxMsg &msg)
//{      
//    if (mRxQueue.size() < mRxQueueSize) 
//    {
//        mRxQueue.push(msg);
//        return true;
//    }
//    return false;
//}
//
//bool CanOnbInterface::readTx(CanTxMsg &msg)
//{
//    if (!mTxQueue.empty())
//    {
//        msg = mTxQueue.front();
//        mTxQueue.pop();
//        return true;
//    }
//    return false;
//}
//
//bool CanOnbInterface::writeTx(CanTxMsg &msg)
//{
//    msg.IDE = CAN_ID_EXT;
//    if (mTxQueue.empty())
//    {
//        if (mCan->send(msg))
//        {
//            return true;
//        }
//    }
//    if (mTxQueue.size() < mTxQueueSize) 
//    {
//        mTxQueue.push(msg);
//        return true;
//    }
//    return false;
//}
//---------------------------------------------------------------------------

void CanOnbInterface::receiveHandler(int fifoNumber, CanRxMsg &canmsg)
{
    if (fifoNumber != mCurFilterFifo)
        return;
    
//    CommonMessage msg;
//    msg.setId(canmsg.ExtId);
//    ByteArray ba(canmsg.Data, canmsg.DLC);
//    msg.setData(std::move(ba));
//    receive(std::move(msg));
    
    receive(CommonMessage(canmsg.ExtId, ByteArray(canmsg.Data, canmsg.DLC)));
}

void CanOnbInterface::setReceiveEnabled(bool enabled)
{
    mCan->setRxInterruptEnabled(enabled);
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
    CanRxMsg canmsg;
    if (mCan->receive(mCurFilterFifo, canmsg))
        receiveHandler(mCurFilterFifo, canmsg);
      
    return ObjnetInterface::read(msg);
}

bool CanOnbInterface::send(const CommonMessage &msg)
{
    if (msg.data().size() > mMaxFrameSize)
        return false;
    CanTxMsg outMsg;
    outMsg.IDE = CAN_ID_EXT;
    outMsg.RTR = 0;//CAN_RTR_Data;
    outMsg.ExtId = msg.rawId();
    outMsg.DLC = msg.data().size();
    for (int i=0; i<outMsg.DLC; i++)
        outMsg.Data[i] = msg.data()[i];
    return mCan->send(outMsg);
}
//---------------------------------------------------------------------------

void CanOnbInterface::flush()
{
    ObjnetInterface::flush();
    mCan->flush(); 
}
//---------------------------------------------------------------------------

int CanOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    int filter = mCan->addFilterB(id, mask, mCurFilterFifo);
//    if (mCurFilterFifo >= 2)
//        mCurFilterFifo = 0;
    return filter;
}

void CanOnbInterface::removeFilter(int number)
{
    mCan->removeFilter(number);
}
//---------------------------------------------------------------------------
