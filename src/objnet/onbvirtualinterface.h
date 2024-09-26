#ifndef _ONBVIRTUALINTERFACE_H
#define _ONBVIRTUALINTERFACE_H

#include "objnetInterface.h"

namespace Objnet
{
    
class OnbVirtualInterfacePool;

class OnbVirtualInterface : public ObjnetInterface
{
public:
    OnbVirtualInterface(OnbVirtualInterfacePool *pool);
    
    virtual bool isBusPresent() const override;
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
protected:
    virtual bool send(const CommonMessage &msg) override;    
    
private:
    uint8_t m_busaddr = 0xFF; // invalid
    OnbVirtualInterfacePool *m_pool = nullptr;
};

class OnbVirtualInterfacePool
{
private:
    friend class OnbVirtualInterface;
    
    bool bindInterface(uint8_t busaddr, OnbVirtualInterface *iface)
    {
        if (busaddr < 16 && !m_interfaces[busaddr])
        {
            m_activeCount++;
            m_interfaces[busaddr] = iface;
            return true;
        }
        return false;
    }
    
    bool unbindInterface(uint8_t busaddr)
    {
        if (busaddr < 16 && m_interfaces[busaddr])
        {
            --m_activeCount;
            m_interfaces[busaddr] = nullptr;
            return true;
        }
        return false;
    }
    
    OnbVirtualInterface *m_interfaces[16] = {0};
    int m_activeCount = 0;
};

}

#endif