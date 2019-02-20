#ifndef _RADIOONBINTERFACE_H
#define _RADIOONBINTERFACE_H

#include "objnet/objnetinterface.h"
#include "radio/cc1200.h"
#include "led.h"

namespace Objnet
{

class RadioOnbInterface : public ObjnetInterface
{
private:
    CC1200 *cc1200;
    Led *ledRx, *ledTx;
    bool mTxBusy, mRxBusy;
    int mRssi, mLqi;
    ByteArray mUnsendBuffer;
    CommonMessage mCurRxMsg, mCurTxMsg;
    CC1200::PayloadHeader mCurHdr;
    unsigned char mCurTxMac;
    bool mBurstRx, mBurstTx;
    bool mTxEnable;
    
    std::queue<CommonMessage> mTxQueue;
    std::queue<CommonMessage> mRxQueue;
    const static int mTxQueueSize = 32;
    const static int mRxQueueSize = 32;
    
    bool writeRx(const CommonMessage &msg);
    bool readTx(CommonMessage &msg);
public: 
    void task();
    void tick(int dt);
    
    int mHdBusyTimeout; // busy timeout for half-duplex mode
    
    bool trySend(ByteArray &ba);

public:
    RadioOnbInterface(CC1200 *device);
    
    bool write(CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();
    
    int availableWriteCount();
    
    int addFilter(unsigned long id, unsigned long mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    void setLeds(Led *rx, Led *tx);
    
    int rssi() const {return mRssi;}
    int lqi() const {return mLqi;}
    
//    bool busPresent();
};

}

#endif