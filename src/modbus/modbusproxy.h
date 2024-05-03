#ifndef MODBUSPROXY_H
#define MODBUSPROXY_H

#include "modbusmaster.h"

class ModbusProxy
{
public:
    explicit ModbusProxy(uint8_t slaveaddr);
    virtual ~ModbusProxy() {}

    uint8_t address() const {return m_address;}

    void setCoilsCount(int count);
    void setInputsCount(int count);

    void readCoils(uint16_t addr, uint16_t count);
    void readDiscreteInputs(uint16_t addr, uint16_t count);
    void readHoldingRegisters(uint16_t addr, uint16_t count);
    void readInputRegisters(uint16_t addr, uint16_t count);
    void writeSingleCoil(uint16_t addr, bool value);
    void writeSingleRegister(uint16_t addr, uint16_t data);
    void writeMultipleCoils(uint16_t addr, uint16_t count, const uint8_t *data);
    void writeMultipleRegisters(uint16_t addr, uint16_t count, uint16_t *data);

protected:
    virtual void errorEvent(Modbus::ExceptionCode e) {}
    virtual void coilReceived(uint16_t addr, bool value) {}
    virtual void discreteInputReceived(uint16_t addr, bool value) {}
    virtual void holdingRegisterReceived(uint16_t addr, uint16_t value) {}
    virtual void inputRegisterReceived(uint16_t addr, uint16_t value) {}

private:
    uint8_t m_address;
    ByteArray m_data;
    ModbusMaster *m_master = nullptr;
    friend class ModbusMaster;
    void parsePDU(uint8_t func, const uint8_t *data, uint8_t size);
    Modbus::ADU createADU(uint8_t func, uint16_t addr, uint16_t value, ByteArray data=ByteArray());
};

#endif // MODBUSPROXY_H
