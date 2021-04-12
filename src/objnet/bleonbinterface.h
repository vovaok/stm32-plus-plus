#ifndef _BLEONBINTERFACE_H
#define _BLEONBINTERFACE_H

#include "objnetInterface.h"
#include "serial/serialinterface.h"

using namespace Serial;

namespace Objnet
{
  
#pragma pack(push,1)
typedef struct
{
    unsigned long id;
    unsigned char size;
    unsigned char data[64];
} BleOnbMessage;
#pragma pack(pop)

class BleOnbInterface : public ObjnetInterface
{
private:
    SerialInterface *mInterface;
    int mReadCnt;
    int mWriteTimer;
    ByteArray mUnsendBuffer;
    BleOnbMessage mCurMsg, mCurTxMsg;
    unsigned char mCurTxMac;
    
    typedef struct
    {
        unsigned long id;
        unsigned long mask;
    } Filter;
    std::vector<Filter> mFilters;
    
    std::queue<BleOnbMessage> mTxQueue;
    std::queue<BleOnbMessage> mRxQueue;
    const static int mTxQueueSize = 160;
    const static int mRxQueueSize = 64;
    
    bool readRx(BleOnbMessage &msg);
    bool writeRx(BleOnbMessage &msg);
    bool readTx(BleOnbMessage &msg);
    bool writeTx(BleOnbMessage &msg);
    
    void task();
    void tick(int dt);
    
    ByteArray mBuffer;
    unsigned char cs, esc, cmd_acc, noSOF;
    ByteArray encode(const ByteArray &ba);
    
    void msgReceived(const ByteArray &ba);
    
    int mHdBusyTimeout; // busy timeout for half-duplex mode
  
public:
    BleOnbInterface(SerialInterface *serialInterface);
  
    bool write(CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();
    
    int availableWriteCount();
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    bool busPresent();
};

};

#endif
