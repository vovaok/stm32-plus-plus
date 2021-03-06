#include "canInterface.h"

using namespace Objnet;

CanInterface::CanInterface(Can *can) :
    mCan(can),
    mCurFilterFifo(0)
{ 
    mMaxFrameSize = 8;
    mBusType = BusCan;
    mCan->setReceiveEvent(EVENT(&CanInterface::receiveHandler));
    mCan->setTransmitReadyEvent(EVENT(&CanInterface::transmitHandler));
}
//---------------------------------------------------------------------------

bool CanInterface::readRx(CanRxMsg &msg)
{
    bool result = false;
    
    mCan->setRxInterruptEnabled(false);
    
    if (!mRxQueue.empty())
    {
        msg = mRxQueue.front();
        mRxQueue.pop();
        result = true;
    }
    else if (mCan->receive(0, msg))
    {
        result = true;
    }
    else if (mCan->receive(1, msg))
    {
        result = true; 
    }
    
    mCan->setRxInterruptEnabled(true);   
    
    return result;
}

bool CanInterface::writeRx(CanRxMsg &msg)
{      
    if (mRxQueue.size() < mRxQueueSize) 
    {
        mRxQueue.push(msg);
        return true;
    }
    return false;
}

bool CanInterface::readTx(CanTxMsg &msg)
{
    if (!mTxQueue.empty())
    {
        msg = mTxQueue.front();
        mTxQueue.pop();
        return true;
    }
    return false;
}

bool CanInterface::writeTx(CanTxMsg &msg)
{
    msg.IDE = CAN_ID_EXT;
    if (mTxQueue.empty())
    {
        if (mCan->send(msg))
        {
            return true;
        }
    }
    if (mTxQueue.size() < mTxQueueSize) 
    {
        mTxQueue.push(msg);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

void CanInterface::receiveHandler(int fifoNumber, CanRxMsg &msg)
{
    writeRx(msg);
}

void CanInterface::transmitHandler()
{
    CanTxMsg msg;
    if (readTx(msg))
        mCan->send(msg);
}
//---------------------------------------------------------------------------

bool CanInterface::write(CommonMessage &msg)
{
    if (msg.data().size() > mMaxFrameSize)
        return false;
    CanTxMsg outMsg;
    outMsg.ExtId = msg.rawId();
    outMsg.DLC = msg.data().size();
    for (int i=0; i<outMsg.DLC; i++)
        outMsg.Data[i] = msg.data()[i];
    return writeTx(outMsg);
}

bool CanInterface::read(CommonMessage &msg)
{
    CanRxMsg inMsg;
    if (!readRx(inMsg))
        return false;
    msg.setId(inMsg.ExtId);
    msg.data().clear();
    msg.data().append(inMsg.Data, inMsg.DLC);
    return true;
}
//---------------------------------------------------------------------------

void CanInterface::flush()
{
    while (!mTxQueue.empty())
        mTxQueue.pop();
    mCan->flush(); 
}
//---------------------------------------------------------------------------

int CanInterface::availableWriteCount()
{
    return mTxQueueSize - mTxQueue.size();
}

int CanInterface::addFilter(uint32_t id, uint32_t mask)
{
    int filter = mCan->addFilter(id, mask, mCurFilterFifo++);
    if (mCurFilterFifo >= 2)
        mCurFilterFifo = 0;
    return filter;
}

void CanInterface::removeFilter(int number)
{
    mCan->removeFilter(number);
}
//---------------------------------------------------------------------------
