#ifndef _UARTONBINTERFACE_H
#define _UARTONBINTERFACE_H

#include "objnetInterface.h"
#include "usart.h"

namespace Objnet
{
  
#pragma pack(push,1)
typedef struct
{
    unsigned long id;
    unsigned char size;
    unsigned char data[8];
} UartOnbMessage;
#pragma pack(pop)

class UartOnbInterface : public ObjnetInterface
{
private:
    Usart *mUsart;
    int mReadCnt;
    UartOnbMessage mCurMsg, mCurTxMsg;
    
    std::queue<UartOnbMessage> mTxQueue;
    std::queue<UartOnbMessage> mRxQueue;
    const static int mTxQueueSize = 32;
    const static int mRxQueueSize = 32;
    
    bool readRx(UartOnbMessage &msg);
    bool writeRx(UartOnbMessage &msg);
    bool readTx(UartOnbMessage &msg);
    bool writeTx(UartOnbMessage &msg);
    
    void task();
    
    ByteArray mBuffer;
    unsigned char cs, esc, cmd_acc;
    ByteArray encode(const ByteArray &ba);
    
    void msgReceived(const ByteArray &ba);
  
public:
    UartOnbInterface(Usart *usart);
  
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