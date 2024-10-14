#ifndef _CANONBINTERFACE_H
#define _CANONBINTERFACE_H

#include "objnetInterface.h"
#include "cansocket.h"

namespace Objnet
{

class CanOnbInterface : public ObjnetInterface
{  
public:
    CanOnbInterface(CanInterface *can);
  
    bool read(CommonMessage &msg);
    void flush();
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    bool busPresent();
    
protected:
    bool send(const CommonMessage &msg);
    
private:
    CanSocket *m_can;
    
    void receiveHandler();
    virtual void setReceiveEnabled(bool enabled);
//    void transmitHandler();
};

};

#endif
