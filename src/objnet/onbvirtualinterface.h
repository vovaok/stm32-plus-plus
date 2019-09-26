#ifndef _ONBVIRTUALINTERFACE_H
#define _ONBVIRTUALINTERFACE_H

#include "objnetInterface.h"

namespace Objnet
{

class OnbVirtualInterface : public ObjnetInterface
{
private:
    typedef struct
    {
        unsigned long id;
        unsigned long mask;
    } Filter;
    std::vector<Filter> mFilters;
    std::vector<OnbVirtualInterface*> mInterfaces;
    std::queue<CommonMessage> mBuffer;
    
    bool testFilter(unsigned long id);
  
public:
    OnbVirtualInterface();
    
    bool write(CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();
    
    int availableWriteCount();
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    void join(OnbVirtualInterface* iface);
};

}

#endif