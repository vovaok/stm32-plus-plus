#pragma once

#include "modbustransport.h"

class Modbus485 : public ModbusTransport
{
public:
    Modbus485(Device *dev);

protected:
    friend class ModbusProxy;
    
    void writeADU(const Modbus::ADU &adu);
    // changed to ModbusTransport::receiveAdu()
//    virtual void parseADU(const Modbus::ADU &adu) = 0;
    virtual void requestSent() {}
    virtual void responseUpdated() {}

    bool isOpen() const {return m_dev->isOpen();}

private:
    Device *m_dev;
    ByteArray m_buffer;
    ByteArray m_outBuffer;
    int m_timeout;

    void task();
    void tick(int dt);
};