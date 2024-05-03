#ifndef MODBUSDEVICE_H
#define MODBUSDEVICE_H

#include "modbus.h"

class ModbusDevice
{
public:
    ModbusDevice(uint8_t address);

    uint8_t address() const {return m_address;}

protected:
    friend class ModbusSlave;
    virtual Modbus::ExceptionCode readCoils(uint16_t addr, uint16_t cnt, uint8_t *data) {return Modbus::eIllegalFunction;}
    virtual Modbus::ExceptionCode readDiscreteInputs(uint16_t addr, uint16_t cnt, uint8_t *data) {return Modbus::eIllegalFunction;}
    virtual Modbus::ExceptionCode readHoldingRegisters(uint16_t addr, uint16_t cnt, uint8_t *data) {return Modbus::eIllegalFunction;}
    virtual Modbus::ExceptionCode readInputRegisters(uint16_t addr, uint16_t cnt, uint8_t *data) {return Modbus::eIllegalFunction;}
    virtual Modbus::ExceptionCode writeSingleCoil(uint16_t addr, uint16_t data) {return Modbus::eIllegalFunction;}
    virtual Modbus::ExceptionCode writeSingleRegister(uint16_t addr, uint16_t data) {return Modbus::eIllegalFunction;}
    virtual Modbus::ExceptionCode writeMultipleCoils(uint16_t addr, uint16_t cnt, const uint8_t *data) {return Modbus::eIllegalFunction;}
    virtual Modbus::ExceptionCode writeMultipleRegisters(uint16_t addr, uint16_t cnt, const uint8_t *data) {return Modbus::eIllegalFunction;}
    
private:
    uint8_t m_address;
};

#endif // MODBUSDEVICE_H
