#pragma once

#include "modbus.h"
#include "modbusdevice.h"
#include "core/coretypes.h"

class ModbusTransport
{
public:    
//    bool readADU(ADU &adu);
    virtual void writeADU(const Modbus::ADU &adu) = 0;

//    NotifyEvent onAduReceived;
    Closure<void(const Modbus::ADU&)> onAduReceived;
    
protected:    
    void receiveADU(const Modbus::ADU &adu);
    
private:
//    ADU m_adu; // no buffer (poka 4to)
};