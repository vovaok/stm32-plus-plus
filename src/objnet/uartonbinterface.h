#ifndef _UARTONBINTERFACE_H
#define _UARTONBINTERFACE_H

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
#warning nado peredelat na ByteArray, a lu4we usat CommonMessage, a ewe lu4we zapilit queue handling into base ObjnetInterface class
    unsigned char data[64];
} UartOnbMessage;
#pragma pack(pop)

class UartOnbInterface : public ObjnetInterface
{
private:
    SerialInterface *mInterface;
    int mReadCnt;
    int mWriteTimer;
    ByteArray mUnsendBuffer;
    UartOnbMessage mCurMsg, mCurTxMsg;
    
    typedef struct
    {
        unsigned long id;
        unsigned long mask;
    } Filter;
    std::vector<Filter> mFilters;
    
    std::queue<UartOnbMessage> mTxQueue;
    std::queue<UartOnbMessage> mRxQueue;
    const static int mTxQueueSize = 64;
    const static int mRxQueueSize = 64;
    
    bool readRx(UartOnbMessage &msg);
    bool writeRx(UartOnbMessage &msg);
    bool readTx(UartOnbMessage &msg);
    bool writeTx(UartOnbMessage &msg);
    
    void task();
    void tick(int dt);
    
    ByteArray mBuffer;
    unsigned char cs, esc, cmd_acc;
    ByteArray encode(const ByteArray &ba);
    
    void msgReceived(const ByteArray &ba);
    
    int mHdBusyTimeout; // busy timeout for half-duplex mode
  
public:
    UartOnbInterface(SerialInterface *serialInterface);
  
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
