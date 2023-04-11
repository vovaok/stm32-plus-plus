#include "genericonbinterface.h"

using namespace Objnet;

static char buf[1024];

GenericOnbInterface::GenericOnbInterface(Device *dev) :
    m_device(dev)
{
    mMaxFrameSize = 1024;
    mBusType = BusEthernet;
    
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
    uint32_t *data = reinterpret_cast<uint32_t *>(buf);
    data[0] = msg.rawId();
    memcpy(data + 1, msg.data().data(), msg.size());
    int sz = msg.size() + 4;
    int written = m_device->write(buf, sz);
    return (sz == written);
}

//static uint32_t onb_id[256];
//static uint8_t onb_id_idx = 0;

void GenericOnbInterface::receiveHandler()
{
//    if (!m_device->isOpen())
//        return false;
    int sz = m_device->read(buf, 1024);
    if (sz < 4)
        return;
    
    uint32_t *data = reinterpret_cast<uint32_t *>(buf);
    uint32_t id = data[0];
    if (testFilter(id))
    {
        CommonMessage msg;
//        if (id & 0x10000000)
//            onb_id[onb_id_idx++] = id;
        msg.setId(id);
        msg.setData(ByteArray(data + 1, sz - 4));
        receive(msg);
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
