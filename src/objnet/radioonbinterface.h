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
//    int mRssi, mLqi;
    char mRssi[16];
    ByteArray mTxBuffer;
    CommonMessage mCurRxMsg, mCurTxMsg;
    CC1200::PayloadHeader mCurHdr;
    unsigned char mCurTxMac;
    bool mBurstRx, mBurstTx;
    int mRxSize;
    unsigned char mAddress;
//    bool mTxEnable;
    const static int mRxBufferSize = 512;
    unsigned char mRxBuffer[mRxBufferSize];
    int mRxHead, mRxTail;
    
    CC1200::Status rfStatus;
    
    int mTimeOutNode,mTimeOutMaster;
    
    enum {stateInit, stateCalibrate, stateIdle, stateTx, stateRx} mState;
    
//    std::queue<CommonMessage> mTxQueue;
//    std::queue<CommonMessage> mRxQueue;
//    const static int mTxQueueSize = 64;
//    const static int mRxQueueSize = 64;
    
    bool writeRx(const CommonMessage &msg);
    bool readTx(CommonMessage &msg);
    
    void masterTask();
    void nodeTask();
    
    void prepareTxBuffer();
    bool parseRxBuffer();
    
    void onRxTxInterrupt();
    void readRxBuffer(unsigned char *data, int size);
    
public: 
    void task();
    void tick(int dt);
    
    int mHdBusyTimeout; // busy timeout for half-duplex mode
    
    bool trySend(ByteArray &ba);

public:
    RadioOnbInterface(CC1200 *device);
    
    bool write(const CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();
    
    int availableWriteCount();
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    void setLeds(Led *rx, Led *tx);
    
    const char *rssi() const {return mRssi;}
    char rssi(unsigned char mac) const {return mRssi[mac];}
//    int lqi() const {return mLqi;}
    
    int errorCount;
    
//    bool busPresent();
};

}

#endif