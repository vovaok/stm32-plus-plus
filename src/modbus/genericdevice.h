#pragma once

#include "modbusdevice.h"
#include "commonproxy.h"
#include <map>

namespace Modbus
{

class GenericDevice : public ModbusDevice
{
public:
    GenericDevice(uint8_t slaveId);
    
    void bindProxy(CommonProxy *proxy, int baseHolding=0, int baseInput=0, int baseCoil=0);
    
    void bindCoil(uint16_t, uint8_t *reg);   
    void bindHoldingRegister(uint16_t addr, uint16_t *reg);
    void bindInputRegister(uint16_t addr, const uint16_t *reg);

protected:
    virtual ExceptionCode readCoils(uint16_t addr, uint16_t cnt, uint8_t *data) override;    
    virtual ExceptionCode readHoldingRegisters(uint16_t addr, uint16_t cnt, uint8_t *data) override;
    virtual ExceptionCode readInputRegisters(uint16_t addr, uint16_t cnt, uint8_t *data) override;
    virtual ExceptionCode writeSingleCoil(uint16_t addr, uint16_t data) override;
    virtual ExceptionCode writeSingleRegister(uint16_t addr, uint16_t data) override;
    virtual ExceptionCode writeMultipleCoils(uint16_t addr, uint16_t cnt, const uint8_t *data) override;
    virtual ExceptionCode writeMultipleRegisters(uint16_t addr, uint16_t cnt,const uint8_t *data) override;
    
private:
    std::map<uint16_t, uint8_t*> m_coils;   
    std::map<uint16_t, uint16_t*> m_holdingRegs;
    std::map<uint16_t, const uint16_t*> m_inputRegs;
    
    struct ProxyDescriptor
    {
        CommonProxy *proxy = nullptr;
        int holdingOffset = 0;
        int inputOffset = 0;
        int coilOffset = 0;
    };
    
    std::vector<ProxyDescriptor> m_proxies;
};

}
