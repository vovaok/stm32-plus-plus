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
    
    std::queue<CanTxMsg> mTxQueue;
    std::queue<CanRxMsg> mRxQueue;
    const static int mTxQueueSize = 32;
    const static int mRxQueueSize = 32;
    
    bool readRx(CanRxMsg &msg);
    bool writeRx(CanRxMsg &msg);
    bool readTx(CanTxMsg &msg);
    bool writeTx(CanTxMsg& msg);
    
    void receiveHandler(int fifoNumber, CanRxMsg &msg);
    void transmitHandler();
  
public:
    CanInterface(Can *can);
  
    bool write(CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();
    
    int availableWriteCount();
    
    int addFilter(unsigned long id, unsigned long mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    bool busPresent();
};

};

#endif
