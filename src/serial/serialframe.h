#ifndef _SERIALFRAME_H
#define _SERIALFRAME_H

#include "core/device.h"

class SerialFrame : public Device
{
public:
    explicit SerialFrame(Device *device);
    Device *getInterface() {return m_device;}

    virtual int bytesAvailable() const override;

protected:
    virtual int writeData(const char *data, int size) override;
    virtual int readData(char *data, int size) override;

private:
    Device *m_device;
    ByteArray m_buffer;
    static const char uartSOF = '{';
    static const char uartESC = '\\';
    static const char uartEOF = '}';

    char cs;
    bool esc, cmd_acc;
    bool m_ready = false;

    void task();
};

#endif
