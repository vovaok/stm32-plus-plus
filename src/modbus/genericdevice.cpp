#include "genericdevice.h"

using namespace Modbus;

GenericDevice::GenericDevice(uint8_t slaveId) : ModbusDevice(slaveId)
{

}
  
void GenericDevice::bindProxy(CommonProxy *proxy, int baseHolding, int baseInput, int baseCoil, Flags defaultFlags)
{
    ProxyDescriptor d;
    d.proxy = proxy;
    d.holdingOffset = proxy->holdingBase() - baseHolding;
    d.inputOffset = proxy->inputBase() - baseInput;
    d.coilOffset = proxy->coilsBase() - baseCoil;
    m_proxies.push_back(d);

    for (int i=0; i<proxy->holdingCount(); i++)
        bindHoldingRegister(baseHolding + i, proxy->holdingRegs() + i, defaultFlags);

    for (int i=0; i<proxy->inputCount(); i++)
        bindInputRegister(baseInput + i, proxy->inputRegs() + i, defaultFlags);

    for (int i=0; i<proxy->coilsCount(); i++)
        bindCoil(baseCoil + i, proxy->coils() + i, defaultFlags);
}

void GenericDevice::bindCoil(uint16_t addr, uint8_t *reg, Flags flags)
{
    m_coils[addr] = {reg, flags};
}

void GenericDevice::bindHoldingRegister(uint16_t addr, uint16_t *reg, Flags flags)
{
    m_holdingRegs[addr] = {reg, flags};
}

void GenericDevice::bindInputRegister(uint16_t addr, const uint16_t *reg, Flags flags)
{
    m_inputRegs[addr] = {reg, static_cast<uint8_t>(flags & ~WriteOnly)};
}

void GenericDevice::setCoilFlags(uint16_t addr, Flags flags)
{
    if (m_coils.count(addr))
        m_coils[addr].flags = flags;
}

void GenericDevice::setHoldingFlags(uint16_t addr, Flags flags)
{
    if (m_holdingRegs.count(addr))
        m_holdingRegs[addr].flags = flags;
}

void GenericDevice::setInputFlags(uint16_t addr, Flags flags)
{
    if (m_inputRegs.count(addr))
        m_inputRegs[addr].flags = flags & ~WriteOnly;
}
//---------------------------------------------------------------------------

ExceptionCode GenericDevice::readCoils(uint16_t addr, uint16_t cnt, uint8_t *data)
{
    uint8_t byte;
    for (int i=0; i<cnt; i++, addr++)
    {
        if (!isReadable(m_coils, addr))
            return eIllegalDataAddress;
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
        if (isReadable(m_holdingRegs, addr))
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
        if (isReadable(m_inputRegs, addr))
            writeWord(data, *m_inputRegs[addr]);
        else
            return eIllegalDataAddress;
    }
    return eNone;
}

ExceptionCode GenericDevice::writeSingleCoil(uint16_t addr, uint16_t data)
{
    if (isWritable(m_coils, addr))
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
        if(onWriteCoilRegs)
           onWriteCoilRegs(addr,1);
        return eNone;
    }
    return eIllegalDataAddress;
}

ExceptionCode GenericDevice::writeSingleRegister(uint16_t addr, uint16_t data)
{
    if (isWritable(m_holdingRegs, addr))
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
        if(onWriteHoldRegs)
           onWriteHoldRegs(addr,1);
        return eNone;
    }
    return eIllegalDataAddress;
}

ExceptionCode GenericDevice::writeMultipleCoils(uint16_t addr, uint16_t cnt, const uint8_t *data)
{    
    for (int i=0,a = addr; i<cnt; i++, a++)
    {
        if (!isWritable(m_coils, a))
            return eIllegalDataAddress;
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
    if(onWriteCoilRegs)
       onWriteCoilRegs(addr,cnt);
    return eNone;
}

ExceptionCode GenericDevice::writeMultipleRegisters(uint16_t addr, uint16_t cnt, const uint8_t *data)
{     
    for (int a = addr,c = cnt; c--; a++)
    {
        if (isWritable(m_holdingRegs, a))
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
    
    if(onWriteHoldRegs)
       onWriteHoldRegs(addr,cnt);
    
    return eNone;
}