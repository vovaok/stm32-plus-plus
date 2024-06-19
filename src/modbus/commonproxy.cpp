#include "commonproxy.h"

CommonProxy::CommonProxy(uint8_t address) :
    ModbusProxy(address)
{

}

CommonProxy::~CommonProxy()
{
    if (m_shadowRegs)
        delete [] m_shadowRegs;
}

void CommonProxy::sync()
{
    // пишем изменившиеся холдинги
    writeChanges();

    // читаем все холдинги
    readHoldingRegisters(m_holdingBase, m_holdingCount);
    
    // читаем все инпуты
    readInputRegisters(m_inputBase, m_inputCount);

    // читаем все койлы
    readCoils(m_coilsBase, m_coilsCount);        
}

void CommonProxy::writeChanges()
{
    int cnt = 0;
    for (int i=0; i<m_holdingCount; i++)
    {
        if (m_shadowRegs[i] != m_holdingRegs[i])
        {
            m_shadowRegs[i] = m_holdingRegs[i];
            cnt++;
        }
        else
        {
          if(cnt)
            writeHoldingRegisters(i - cnt + m_holdingBase, cnt);
            cnt = 0;
        }
    }
    if (cnt)
        writeHoldingRegisters(m_holdingCount - cnt + m_holdingBase, cnt);
}

void CommonProxy::writeHoldingRegisters(uint16_t addr, int count)
{
    uint16_t *data = m_holdingRegs + (addr - m_holdingBase);
    if (count == 1)
        writeSingleRegister(addr, *data);
    else if (count > 1)
        writeMultipleRegisters(addr, count, data);
}

void CommonProxy::errorEvent(Modbus::ExceptionCode e)
{
    if (m_validityTimeout)
        --m_validityTimeout;
}

void CommonProxy::coilReceived(uint16_t addr, bool value)
{
    int offset = (int)addr - (int)m_coilsBase;
    if (offset >= 0 && offset < m_coilsCount)
        m_coils[offset] = value;

    m_validityTimeout = 4;
}

void CommonProxy::holdingRegisterReceived(uint16_t addr, uint16_t value)
{
    int idx = (int)addr - (int)m_holdingBase;
    if (idx >= 0 && idx < m_holdingCount)
    {
        // если регистр не был изменён в локальной копии
        // то переписываем новое значение и туда, и туда
        if (m_holdingRegs[idx] == m_shadowRegs[idx])
            m_holdingRegs[idx] = m_shadowRegs[idx] = value;
        // в противном случае будет выполнена запись
        // во время следующей синхронизации
    }
}

void CommonProxy::inputRegisterReceived(uint16_t addr, uint16_t value)
{
    int idx = (int)addr - (int)m_inputBase;
    if (idx >= 0 && idx < m_inputCount)
        m_inputRegs[idx] = value;
}

uint16_t CommonProxy::holdingRegAddr(void *ptr)
{
    int idx = reinterpret_cast<uint16_t*>(ptr) - m_holdingRegs;
    if (idx >= 0 && idx < m_holdingCount)
        return m_holdingBase + idx;
    return 0;
}

void CommonProxy::readRegs(void *ptr, int count)
{
    uint16_t addr = holdingRegAddr(ptr);
    if (addr)
        readHoldingRegisters(addr, count);
    else
    {
        int idx = reinterpret_cast<uint16_t*>(ptr) - m_inputRegs;
        if (idx >= 0 && idx < m_inputCount)
            readInputRegisters(m_inputBase + idx, count);
    }
}

void CommonProxy::writeRegs(void *ptr, int count, void *data)
{
    uint16_t addr = holdingRegAddr(ptr);
    if (addr)
    {
//        for (int i=0; i<count; i++)
//            reinterpret_cast<uint16_t *>(ptr)[i] = reinterpret_cast<uint16_t *>(data)[i];

        if (count == 1)
            writeSingleRegister(addr, *reinterpret_cast<uint16_t *>(data));
        else if (count > 1)
            writeMultipleRegisters(addr, count, reinterpret_cast<uint16_t *>(data));
    }
}
