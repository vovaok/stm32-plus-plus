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
    
    enum Flags : uint8_t
    {
        NoAccess = 0x00,
        ReadOnly = 0x01,
        WriteOnly = 0x02,
        ReadWrite = 0x03
    };
    
    void bindProxy(CommonProxy *proxy, int baseHolding=0, int baseInput=0, int baseCoil=0, Flags defaultFlags=ReadWrite);
    
    void bindCoil(uint16_t, uint8_t *reg, Flags flags=ReadWrite);   
    void bindHoldingRegister(uint16_t addr, uint16_t *reg, Flags flags=ReadWrite);
    void bindInputRegister(uint16_t addr, const uint16_t *reg, Flags=ReadOnly);
    
    std::function<void(short,short)> onWriteHoldRegs;
    std::function<void(short,short)> onWriteCoilRegs;
    
    void setCoilFlags(uint16_t addr, Flags flags);
    void setHoldingFlags(uint16_t addr, Flags flags);
    void setInputFlags(uint16_t addr, Flags flags);
    
    void setDebugMode(bool enabled) {m_debugMode = enabled;}

protected:
    virtual ExceptionCode readCoils(uint16_t addr, uint16_t cnt, uint8_t *data) override;    
    virtual ExceptionCode readHoldingRegisters(uint16_t addr, uint16_t cnt, uint8_t *data) override;
    virtual ExceptionCode readInputRegisters(uint16_t addr, uint16_t cnt, uint8_t *data) override;
    virtual ExceptionCode writeSingleCoil(uint16_t addr, uint16_t data) override;
    virtual ExceptionCode writeSingleRegister(uint16_t addr, uint16_t data) override;
    virtual ExceptionCode writeMultipleCoils(uint16_t addr, uint16_t cnt, const uint8_t *data) override;
    virtual ExceptionCode writeMultipleRegisters(uint16_t addr, uint16_t cnt,const uint8_t *data) override;
    
private:    
    template <typename T>
    class Entry
    {
    public:
        T *ptr;
        uint8_t flags;
        T &operator*() {return *ptr;}
        inline bool isReadable() const {return flags & ReadOnly;}
        inline bool isWritable() const {return flags & WriteOnly;}
    };
    
    typedef Entry<uint8_t> Coil;
    typedef Entry<uint16_t> Holding;
    typedef Entry<const uint16_t> Input;
    
    std::map<uint16_t, Coil> m_coils;   
    std::map<uint16_t, Holding> m_holdingRegs;
    std::map<uint16_t, Input> m_inputRegs;
    
    template <typename T>
    bool isReadable(const T &mapa, uint16_t addr)
    {
        return mapa.count(addr) && (mapa.at(addr).isReadable() || m_debugMode);
    }
    
    template <typename T>
    bool isWritable(const T &mapa, uint16_t addr)
    {
        return mapa.count(addr) && (mapa.at(addr).isWritable() || m_debugMode);
    }
    
    struct ProxyDescriptor
    {
        CommonProxy *proxy = nullptr;
        int holdingOffset = 0;
        int inputOffset = 0;
        int coilOffset = 0;
    };
    
    std::vector<ProxyDescriptor> m_proxies;
    
    bool m_debugMode = false; // ignore access flags
};

}
