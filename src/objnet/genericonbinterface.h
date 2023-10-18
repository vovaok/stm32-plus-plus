#ifndef _GENERICONBINTERFACE_H
#define _GENERICONBINTERFACE_H

#include "objnetInterface.h"
#include "core/device.h"

namespace Objnet
{

class GenericOnbInterface : public ObjnetInterface
{
public:
    GenericOnbInterface(Device *dev);
    virtual ~GenericOnbInterface() {}
    
    virtual bool isBusPresent() const {return m_device->isOpen();}
    
//    void flush();
    
    int availableWriteCount();
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    const Device *device() {return m_device;}
    
protected:
    const uint32_t *prefix = nullptr;
    
    virtual bool send(const CommonMessage &msg);
    void receiveHandler();
  
private:
    Device *m_device;
        
    typedef struct
    {
        unsigned long id;
        unsigned long mask;
    } Filter;
    std::vector<Filter> mFilters;
    
    bool testFilter(unsigned long id);
    void onDeviceRead();
};

}

#endif