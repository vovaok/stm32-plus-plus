#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include "modbustransport.h"
#include <map>

class ModbusDevice;

class ModbusSlave
{
public:
    ModbusSlave(ModbusTransport *transport);
    void bindDevice(ModbusDevice *dev);

protected:
    void task();

private:
    ModbusTransport *m_transport;
    std::map<uint8_t, ModbusDevice*> m_devices;
    int m_address;
    ByteArray m_outData;

    void parseADU(const Modbus::ADU &adu);

    void sendException(uint8_t addr, Modbus::ExceptionCode e, Modbus::FunctionCode f);
};

#endif // MODBUSSLAVE_H
