#include "onbvirtualinterface.h"

using namespace Objnet;

OnbVirtualInterface::OnbVirtualInterface()
{
    mMaxFrameSize = 256;
}

bool OnbVirtualInterface::testFilter(unsigned long id)
{
    int cnt = mFilters.size();
    for (int i=0; i<cnt; i++)
    {
        if ((id & mFilters[i].mask) == mFilters[i].id)
            return true;
    }
    return false;
}

bool OnbVirtualInterface::write(CommonMessage &msg)
{
    int cnt = mInterfaces.size();
    for (int i=0; i<cnt; i++)
    {
        OnbVirtualInterface *iface = mInterfaces[i];
        if (iface->testFilter(msg.rawId()))
            iface->mBuffer.push(msg);
    }
    return true;
}

bool OnbVirtualInterface::read(CommonMessage &msg)
{
    if (mBuffer.empty())
        return false;
    
    msg = mBuffer.front();
    mBuffer.pop();
    return true;
}

void OnbVirtualInterface::flush()
{
  
}

int OnbVirtualInterface::availableWriteCount()
{
    return 1; // x3 norm ili net
}

int OnbVirtualInterface::addFilter(unsigned long id, unsigned long mask)
{
    Filter f = {id & 0x1FFFFFFF, mask & 0x1FFFFFFF};
    mFilters.push_back(f);
    return mFilters.size();
}

void OnbVirtualInterface::removeFilter(int number)
{
    if (number >= 0 && number < mFilters.size())
        mFilters.erase(mFilters.begin() + number);
}

void OnbVirtualInterface::join(OnbVirtualInterface* iface)
{
    if (iface == this)
        return;
    for (int i=0; i<mInterfaces.size(); i++)
    {
        if (mInterfaces[i] == iface)
            return;
    }
    mInterfaces.push_back(iface);
    iface->mInterfaces.push_back(this);
    for (int i=0; i<mInterfaces.size(); i++)
        mInterfaces[i]->join(iface);
    for (int i=0; i<iface->mInterfaces.size(); i++)
        join(iface->mInterfaces[i]);
}