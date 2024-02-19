#ifndef _UARTONBINTERFACE_H
#define _UARTONBINTERFACE_H

#include "objnetInterface.h"
#include "core/device.h"
#include <array>

namespace Objnet
{

class UartOnbInterface : public ObjnetInterface
{
public:
    UartOnbInterface(Device *serialInterface);
    virtual ~UartOnbInterface();

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);

    bool busPresent();

protected:
    virtual bool send(const CommonMessage &msg);

private:
    Device *m_device;
    uint8_t mCurTxMac;
    bool m_busy = false;

    struct Filter
    {
        uint32_t id;
        uint32_t mask;
    } mFilters[8];
    int m_filterCount = 0;

    void task();
    void tick(int dt);

    ByteArray mBuffer;
    char *m_sendBuffer = nullptr;

    void msgReceived(const ByteArray &ba);

    int mHdBusyTimeout; // busy timeout for half-duplex mode
};

};

#endif
