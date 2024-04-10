#pragma once

#include "core/device.h"

class DeviceFramed : public Device
{
public:
    DeviceFramed(Device *dev);

    virtual int bytesAvailable() const override;

protected:
    virtual int writeData(const char *data, int size) override;
    virtual int readData(char *data, int size) override;

private:
    Device *m_device;
    ByteArray m_rxBuffer;
};