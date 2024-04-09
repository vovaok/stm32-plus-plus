#include "onbvirtualinterface.h"

using namespace Objnet;

OnbVirtualInterface::OnbVirtualInterface(OnbVirtualInterfacePool *pool) :
    m_pool(pool)
{
    mMaxFrameSize = 256;
    mBusType = BusVirtual;
}

bool OnbVirtualInterface::isBusPresent() const
{
    return m_pool->m_activeCount > 1;
}

bool OnbVirtualInterface::send(const CommonMessage &msg)
{
    bool result = true;
    for (int i=0; i<16; i++)
    {
        OnbVirtualInterface *iface = m_pool->m_interfaces[i];
        if (!iface || iface == this)
            continue;
        
        if (msg.isGlobal() || (msg.isLocal() && msg.localId().mac == iface->m_busaddr))
        {
            CommonMessage mess = msg; // copy popy
            result &= iface->receive(std::move(mess));
        }
    }
    return result;
}

int OnbVirtualInterface::addFilter(uint32_t id, uint32_t mask)
{
    if (id & 0x10000000)
    {
        m_busaddr = (id >> 24) & 0xF;
        m_pool->bindInterface(m_busaddr, this);
    }
    return 1; // x3
}

void OnbVirtualInterface::removeFilter(int number)
{
    m_pool->unbindInterface(m_busaddr);
}
