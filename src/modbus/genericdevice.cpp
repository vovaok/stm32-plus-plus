#include "genericdevice.h"

using namespace Modbus;

GenericDevice::GenericDevice(uint8_t slaveId) : ModbusDevice(slaveId)
{

}
  
void GenericDevice::bindProxy(CommonProxy *proxy, int baseHolding, int baseInput, int baseCoil)
{
    ProxyDescriptor d;
    d.proxy = proxy;
    d.holdingOffset = proxy->holdingBase() - baseHolding;
    d.inputOffset = proxy->inputBase() - baseInput;
    d.coilOffset = proxy->coilsBase() - baseCoil;
    m_proxies.push_back(d);

    for (int i=0; i<proxy->holdingCount(); i++)
        bindHoldingRegister(baseHolding + i, proxy->holdingRegs() + i);

    for (int i=0; i<proxy->inputCount(); i++)
        bindInputRegister(baseInput + i, proxy->inputRegs() + i);

    for (int i=0; i<proxy->coilsCount(); i++)
        bindCoil(baseCoil + i, proxy->coils() + i);
}

void GenericDevice::bindCoil(uint16_t addr, uint8_t *reg)
{
    m_coils[addr] = reg;
}

void GenericDevice::bindHoldingRegister(uint16_t addr, uint16_t *reg)
{
    m_holdingRegs[addr] = reg;
}

void GenericDevice::bindInputRegister(uint16_t addr, const uint16_t *reg)
{
    m_inputRegs[addr] = reg;
}
//---------------------------------------------------------------------------

ExceptionCode GenericDevice::readCoils(uint16_t addr, uint16_t cnt, uint8_t *data)
{
    uint8_t byte;
    for (int i=0; i<cnt; i++, addr++)
    {
        if (!m_coils.count(addr)) return eIllegalDataAddress;
        int bit = i & 7;
        if (!bit)
            byte = 0;
        if (*m_coils[addr])
            byte |= 1 << bit;
        data[i >> 3] = byte;
    }
    return eNone;  
}

ExceptionCode GenericDevice::readHoldingRegisters(uint16_t addr, uint16_t cnt, uint8_t *data)
{
    for (; cnt--; addr++)
    {
        if (m_holdingRegs.count(addr))
            writeWord(data, *m_holdingRegs[addr]);
        else
            return eIllegalDataAddress;
    }
    return eNone;
}

ExceptionCode GenericDevice::readInputRegisters(uint16_t addr, uint16_t cnt, uint8_t *data)
{
    for (; cnt--; addr++)
    {
        if (m_inputRegs.count(addr))
            writeWord(data, *m_inputRegs[addr]);
        else
            return eIllegalDataAddress;
    }
    return eNone;
}

ExceptionCode GenericDevice::writeSingleCoil(uint16_t addr, uint16_t data)
{
    if (m_coils.count(addr))
    {      
        switch (data)
        {
          case 0x0000: *m_coils[addr] = 0; break;
          case 0xFF00: *m_coils[addr] = 1; break;
          default: return eIllegalDataValue;
        }   

        for (ProxyDescriptor &d: m_proxies)
        {
            int mappedAddr = addr + d.coilOffset;
            int idx = mappedAddr - d.proxy->coilsBase();
            if (idx >= 0 && idx < d.proxy->coilsCount())
            {
                d.proxy->writeSingleCoil(mappedAddr, *m_coils[addr]); 
                break;
            }
        }
        return eNone;
    }
    return eIllegalDataAddress;
}

ExceptionCode GenericDevice::writeSingleRegister(uint16_t addr, uint16_t data)
{
    if (m_holdingRegs.count(addr))
    {    
        *m_holdingRegs[addr] = data;       
    
        for (ProxyDescriptor &d: m_proxies)
        {
            int mappedAddr = addr + d.holdingOffset;
            int idx = mappedAddr - d.proxy->holdingBase();
            if (idx >= 0 && idx < d.proxy->holdingCount())
            {
                d.proxy->writeHoldingRegisters(mappedAddr, 1);
                break;
            }
        }
        return eNone;
    }
    return eIllegalDataAddress;
}

ExceptionCode GenericDevice::writeMultipleCoils(uint16_t addr, uint16_t cnt, const uint8_t *data)
{    
    for (int i=0,a = addr; i<cnt; i++, a++)
    {
       if (!m_coils.count(a)) return eIllegalDataAddress;
        int bit = i & 7;
        if (data[i >> 3] & (1 << bit))
            *m_coils[a] = 1;
        else
            *m_coils[a] = 0;
    }
    
    for (ProxyDescriptor &d: m_proxies)
    {
        int mappedAddr = addr + d.coilOffset;
        int idx = mappedAddr - d.proxy->coilsBase();
        if (idx >= 0 && idx < d.proxy->coilsCount())
        {
            int maxcnt = d.proxy->coilsCount() - idx;
            if (cnt > maxcnt)
                cnt = maxcnt;
            //! @todo check validity of data
            d.proxy->writeMultipleCoils(mappedAddr, cnt, data);
            break;
        }
    }
    
    return eNone;
}

ExceptionCode GenericDevice::writeMultipleRegisters(uint16_t addr, uint16_t cnt, const uint8_t *data)
{     
    for (int a = addr,c = cnt; c--; a++)
    {
        if (m_holdingRegs.count(a))
            *m_holdingRegs[a] = readWord(data);
        else
            return eIllegalDataAddress; 
    } 
    
    //! @todo implement writing several proxies at once

    for (ProxyDescriptor &d: m_proxies)
    {
        int mappedAddr = addr + d.holdingOffset;
        int idx = mappedAddr - d.proxy->holdingBase();
        if (idx >= 0 && idx < d.proxy->holdingCount())
        {
            int maxcnt = d.proxy->holdingCount() - idx;
            if (cnt > maxcnt)
                cnt = maxcnt;
            d.proxy->writeHoldingRegisters(mappedAddr, cnt);
            break;
        }
    }
    
    return eNone;
}