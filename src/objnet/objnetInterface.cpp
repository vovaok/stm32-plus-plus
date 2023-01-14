#include "objnetInterface.h"

using namespace Objnet;

ObjnetInterface::ObjnetInterface() :
    mMaxFrameSize(8),
    mBusType(BusUnknown),
    isMaster(false),
    mTxQueueSize(32),
    mRxQueueSize(32)
{
#if !defined(QT_CORE_LIB)
    stmApp()->registerTaskEvent(EVENT(&ObjnetInterface::task));
#endif
}

void ObjnetInterface::task()
{
    while (mTxQueue.size())
    {
        CommonMessage &msg = mTxQueue.front();
        if (send(msg))
            mTxQueue.pop();
        else
            break;
    }
}

int ObjnetInterface::availableWriteCount()
{
    return mTxQueueSize - mTxQueue.size();
}

void ObjnetInterface::flush()
{
    while (!mTxQueue.empty())
        mTxQueue.pop();
}

bool ObjnetInterface::write(const CommonMessage &msg)
{
    if (mTxQueue.empty())
    {
        if (send(msg))
            return true;
    }
    if (mTxQueue.size() < mTxQueueSize - 1)
    {
        mTxQueue.push(msg);
        return true;
    }
    return false;
}
   
bool ObjnetInterface::read(CommonMessage &msg)
{
    if (mRxQueue.size())
    {
        setReceiveEnabled(false);
        msg = mRxQueue.front();
        mRxQueue.pop();
        setReceiveEnabled(true);
        return true;
    }
    return false;
}

bool ObjnetInterface::receive(const CommonMessage &msg)
{
    if (mRxQueue.size() < mRxQueueSize - 1)
    {
        mRxQueue.push(msg);
        return true;
    }
    return false;
}
