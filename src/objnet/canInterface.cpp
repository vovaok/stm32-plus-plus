#include "canInterface.h"

using namespace Objnet;

CanInterface::CanInterface(Can *can) :
    mCan(can),
    mCurFilterFifo(0)
{ 
    mMaxFrameSize = 8;
    mCan->setReceiveEvent(EVENT(&CanInterface::receiveHandler));
    mCan->setTransmitReadyEvent(EVENT(&CanInterface::transmitHandler));
}
//---------------------------------------------------------------------------

bool CanInterface::readRx(CanRxMsg &msg)
{
    if (!mRxQueue.empty())
    {
        msg = mRxQueue.front();
        mRxQueue.pop();
        return true;
    }
    else if (mCan->receive(0, msg))
    {
        return true;
    }
    else if (mCan->receive(1, msg))
    {
        return true; 
    }
    return false;
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

int CanInterface::addFilter(unsigned long id, unsigned long mask)
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
