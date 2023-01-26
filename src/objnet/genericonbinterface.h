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
    
//    void flush();
    
    int availableWriteCount();
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
protected:
    virtual bool send(const CommonMessage &msg);
  
private:
    Device *m_device;
  
    typedef struct
    {
        unsigned long id;
        unsigned long mask;
    } Filter;
    std::vector<Filter> mFilters;
    
    bool testFilter(unsigned long id);
    
    void receiveHandler();
};

}

#endif