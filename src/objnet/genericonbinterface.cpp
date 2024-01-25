#include "genericonbinterface.h"

using namespace Objnet;

static char buf[1024];

GenericOnbInterface::GenericOnbInterface(Device *dev) :
    m_device(dev)
{
    mMaxFrameSize = 1024;
    mBusType = BusUnknown;
    
    m_device->onReadyRead = EVENT(&GenericOnbInterface::receiveHandler);
    m_device->open();
}

bool GenericOnbInterface::testFilter(unsigned long id)
{
    int cnt = mFilters.size();
    for (int i=0; i<cnt; i++)
    {
        if ((id & mFilters[i].mask) == mFilters[i].id)
            return true;
    }
    return false;
}

bool GenericOnbInterface::send(const CommonMessage &msg)
{
    if (!m_device->isOpen())
        return false;
    
    uint32_t *data = reinterpret_cast<uint32_t *>(buf);
    int sz = msg.size();
    if (prefix)
    {
        *data++ = *prefix;
        sz += 4;
    }
    *data++ = msg.rawId();
    sz += 4;
    memcpy(data, msg.data().data(), msg.size());
    int written = m_device->write(buf, sz);
    return (sz == written);
}

void GenericOnbInterface::receiveHandler()
{
//    if (!m_device->isOpen())
//        return false;
    int sz = m_device->read(buf, 1024);
    
    uint32_t *data = reinterpret_cast<uint32_t *>(buf);
    if (prefix)
    {
        if (sz < 4 || *data++ != *prefix)
            return;
        sz -= 4;
    }
    
    if (sz < 4)
        return;
    
    uint32_t id = *data++;
    sz -= 4;
    if (testFilter(id))
    {
        ByteArray ba = ByteArray::fromRawData(reinterpret_cast<const char *>(data), sz);
        CommonMessage msg(id, std::move(ba));
        receive(std::move(msg));
    }
}

//void GenericOnbInterface::flush()
//{
//    if (m_device->isOpen())
//        while (m_device->read(buf, 1024));
//}

int GenericOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    Filter f = {id & 0x1FFFFFFF, mask & 0x1FFFFFFF};
    mFilters.push_back(f);
    return mFilters.size();
}

void GenericOnbInterface::removeFilter(int number)
{
    if (number >= 0 && number < mFilters.size())
        mFilters.erase(mFilters.begin() + number);
}
