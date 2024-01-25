#include "objnetInterface.h"

using namespace Objnet;

ObjnetInterface::ObjnetInterface() :
    mMaxFrameSize(8),
    mBusType(BusUnknown),
    isMaster(false),
    mTxQueue(32),
    mRxQueue(32)
{
#if !defined(QT_CORE_LIB)
    stmApp()->registerTaskEvent(EVENT(&ObjnetInterface::task));
#endif
}

void ObjnetInterface::task()
{
    if (!isBusPresent())
        return;

    while (!mTxQueue.isEmpty())
    {
        const CommonMessage &msg = mTxQueue.front();
        if (send(msg))
            mTxQueue.pop_front();
        else
            break;
    }
}

int ObjnetInterface::availableWriteCount()
{
    return mTxQueue.maxsize() - mTxQueue.size();
}

void ObjnetInterface::flush()
{
    while (!mTxQueue.isEmpty())
        mTxQueue.pop_front();
}

bool ObjnetInterface::write(const CommonMessage &msg)
{
    if (mTxQueue.isEmpty())
    {
        if (send(msg))
            return true;
    }
    if (mTxQueue.size() < mTxQueue.maxsize() - 1)
    {
        mTxQueue.push_back(msg);
        return true;
    }
    return false;
}
   
bool ObjnetInterface::read(CommonMessage &msg)
{
    if (!mRxQueue.isEmpty())
    {
        setReceiveEnabled(false);
        msg = mRxQueue.front();
        mRxQueue.pop_front();
        setReceiveEnabled(true);
        return true;
    }
    return false;
}

const CommonMessage *ObjnetInterface::peekNext()
{
    if (!mRxQueue.isEmpty())
        return &mRxQueue.front();
    return nullptr;
}

void ObjnetInterface::discardNext()
{
    mRxQueue.pop_front();
}

//bool ObjnetInterface::receive(const CommonMessage &msg)
//{
//    if (mRxQueue.size() < mRxQueue.maxsize() - 1)
//    {
//        mRxQueue.push_back(msg);
//        if (onReceive)
//            onReceive();
//        return true;
//    }
//    return false;
//}

bool ObjnetInterface::receive(CommonMessage &&msg)
{
    if (mRxQueue.size() < mRxQueue.maxsize() - 1)
    {
        mRxQueue.push_back(std::move(msg));
        if (onReceive)
            onReceive();
        return true;
    }
    return false;
}
