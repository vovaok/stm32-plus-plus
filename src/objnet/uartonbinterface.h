#ifndef _UARTONBINTERFACE_H
#define _UARTONBINTERFACE_H

#include "objnetInterface.h"
#include "core/device.h"

namespace Objnet
{

class UartOnbInterface : public ObjnetInterface
{  
public:
    UartOnbInterface(Device *serialInterface);
    
    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);
    
    bool busPresent();
    
protected:
    virtual bool send(const CommonMessage &msg);
    
private:
    Device *m_device;
//    int mWriteTimer;
//    ByteArray mUnsendBuffer;
//    UartOnbMessage mCurMsg, mCurTxMsg;
    unsigned char mCurTxMac;
    
    typedef struct
    {
        unsigned long id;
        unsigned long mask;
    } Filter;
    std::vector<Filter> mFilters;
    
    void task();
    void tick(int dt);
    
    ByteArray mBuffer;
    unsigned char cs, esc, cmd_acc, noSOF;
    void decode(const char *data, int size);
    ByteArray encode(const ByteArray &ba);
    
    void msgReceived(const ByteArray &ba);
    
    int mHdBusyTimeout; // busy timeout for half-duplex mode
};

};

#endif
