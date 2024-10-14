#include "modbusproxy.h"
#include "modbusmaster.h"

ModbusProxy::ModbusProxy(uint8_t slaveaddr) :
    m_address(slaveaddr)
{
}

void ModbusProxy::readCoils(uint16_t addr, uint16_t count)
{
    m_master->writeADU(createADU(Modbus::cmdReadCoils, addr, count));
}

void ModbusProxy::readDiscreteInputs(uint16_t addr, uint16_t count)
{
    m_master->writeADU(createADU(Modbus::cmdReadDiscreteInputs, addr, count));
}

void ModbusProxy::readHoldingRegisters(uint16_t addr, uint16_t count)
{
    m_master->writeADU(createADU(Modbus::cmdReadHoldingRegisters, addr, count));
}

void ModbusProxy::readInputRegisters(uint16_t addr, uint16_t count)
{
    m_master->writeADU(createADU(Modbus::cmdReadInputRegisters, addr, count));
}

void ModbusProxy::writeSingleCoil(uint16_t addr, bool value)
{
    m_master->writeADU(createADU(Modbus::cmdWriteSingleCoil, addr, (value? 0xFF00: 0x0000)));
}

void ModbusProxy::writeSingleRegister(uint16_t addr, uint16_t data)
{
    m_master->writeADU(createADU(Modbus::cmdWriteSingleRegister, addr, data));
}

void ModbusProxy::writeMultipleCoils(uint16_t addr, uint16_t count, const uint8_t *data)
{
    uint8_t N = (count + 7) / 8;
    ByteArray ba;
    ba.append(N);
    ba.append(reinterpret_cast<const char*>(data), N);
    m_master->writeADU(createADU(Modbus::cmdWriteMultipleCoils, addr, count, ba));
}

void ModbusProxy::writeMultipleRegisters(uint16_t addr, uint16_t count, uint16_t *data)
{
    uint8_t N = count * 2;
    ByteArray ba;
    ba.resize(N + 1);
    ba[0] = N;
    uint8_t *ptr = reinterpret_cast<uint8_t*>(ba.data() + 1);
    for (int i=0; i<count; i++)
        Modbus::writeWord(ptr, data[i]);
    m_master->writeADU(createADU(Modbus::cmdWriteMultipleRegisters, addr, count, ba));
}

void ModbusProxy::parsePDU(uint8_t func, const uint8_t *data, uint8_t size)
{
    uint8_t N;

    uint16_t addr = m_master->getCurrentRequestAddress();
    uint16_t quantity = m_master->getCurrentRequestQuantity();

    switch (func)
    {
    case Modbus::cmdReadCoils:
        N = *data++; // fuck this useless byte count
        if (quantity > N*8)
            return; // fail
        for (int i=0; i<quantity; i++)
        {
            int mask = 1 << (i & 7);
            int byte = i >> 3;
            coilReceived(addr++, data[byte] & mask);
        }
        break;

      case Modbus::cmdReadDiscreteInputs:
        N = *data++;
        if (quantity > N*8)
            return; // fail
        for (int i=0; i<quantity; i++)
        {
            int mask = 1 << (i & 7);
            int byte = i >> 3;
            discreteInputReceived(addr++, data[byte] & mask);
        }
        break;

      case Modbus::cmdReadHoldingRegisters:
        N = *data++;
        for (int i=0; i<N/2; i++)
        {
            uint16_t value = Modbus::readWord(data);
            holdingRegisterReceived(addr++, value);
        }
        break;

      case Modbus::cmdReadInputRegisters:
        N = *data++;
        for (int i=0; i<N/2; i++)
        {
            uint16_t value = Modbus::readWord(data);
            inputRegisterReceived(addr++, value);
        }
        break;

      case Modbus::cmdWriteSingleCoil:
        break;

      case Modbus::cmdWriteSingleRegister:
        break;

      case Modbus::cmdReadExceptionStatus:
        break;

      case Modbus::cmdDiagnostics:
        break;

      case Modbus::cmdGetCommEventCounter:
        break;

      case Modbus::cmdGetCommEventLog:
        break;

      case Modbus::cmdWriteMultipleCoils:
        break;

      case Modbus::cmdWriteMultipleRegisters:
        break;

      case Modbus::cmdReportSlaveId:
        break;

      case Modbus::cmdReadFileRecord:
        break;

      case Modbus::cmdWriteFileRecord:
        break;

      case Modbus::cmdMaskWriteRegister:
        break;

      case Modbus::cmdReadWriteMultipleRegisters:
        break;

      case Modbus::cmdReadFifoQueue:
        break;
    }
}

Modbus::ADU ModbusProxy::createADU(uint8_t func, uint16_t addr, uint16_t value, ByteArray data)
{
    m_data.resize(4);
    uint8_t *ptr = (uint8_t*)m_data.data();
    Modbus::writeWord(ptr, addr);
    Modbus::writeWord(ptr, value);
    m_data.append(data);

    Modbus::ADU adu;
    adu.addr = m_address;
    adu.func = func;
    adu.size = m_data.size();
    adu.data = m_data.data();
    return adu;
}
