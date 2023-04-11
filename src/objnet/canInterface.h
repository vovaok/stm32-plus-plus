#ifndef _CAN_INTERFACE_H
#define _CAN_INTERFACE_H

#include "objnetInterface.h"
#include "can.h"

namespace Objnet
{

class CanInterface : public ObjnetInterface
{
private:
    Can *mCan;
    int mCurFilterFifo;
    
    void receiveHandler(int fifoNumber, CanRxMsg &canmsg);
    virtual void setReceiveEnabled(bool enabled);
//    void transmitHandler();
  
public:
    CanInterface(Can *can, int fifoNumber=0);
  
    bool read(CommonMessage &msg);
    void flush();
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    bool busPresent();
    
protected:
    bool send(const CommonMessage &msg);
};

};

#endif
