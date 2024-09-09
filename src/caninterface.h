#pragma once

#include <stdint.h>
#include <vector>
#include "core/device.h"

class CanSocket;

class CanInterface
{
public:
    CanInterface(int fifoCount);
    
    enum Flags
    {
        StdId = 0x00,
        ExtId = 0x01,
        FD    = 0x02, // CAN FD mode
        BRS   = 0x04, // bit rate switch
    };
    
    virtual bool hasFD() {return false;}
    
public:
    virtual int configureFilter(Flags flags, uint32_t id, uint32_t mask, int fifoChannel) = 0;
    virtual bool removeFilter(int index) = 0;

    virtual int pendingMessageLength(int fifoChannel) = 0;
    virtual int receiveMessage(uint32_t *id, uint8_t *data, uint8_t maxsize, int fifoChannel) = 0;
    virtual bool transmitMessage(Flags flags, uint32_t id, const uint8_t *data, uint8_t size) = 0;

    virtual bool open(Device::OpenMode mode=Device::ReadWrite) = 0; // set to normal mode
    virtual bool close() = 0; // set to init mode
    
    virtual void setRxInterruptEnabled(int fifoChannel, bool enabled) {}
    
//    virtual bool setBaudrate(int value) = 0;
   
protected:    
    friend class CanSocket;
    
    int acquireFifoChannel(CanSocket *socket=nullptr);
    void releaseFifoChannel(int fifoChannel);
    
    // call this when new message is available
    void messageReceived(int fifoChannel);
    
    static uint8_t dlcFromSize(uint8_t size);
    
private:
    uint32_t m_availFifo;
    std::vector<CanSocket *> m_sockets;
};
