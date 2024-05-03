#include "modbusslave.h"
#include "modbusdevice.h"
#include "core/core.h"

using namespace Modbus;

ModbusSlave::ModbusSlave(ModbusTransport *transport) :
    m_transport(transport)
{
    m_transport->onAduReceived = EVENT(&ModbusSlave::parseADU);
}

void ModbusSlave::bindDevice(ModbusDevice *dev)
{
    m_devices[dev->address()] = dev;
}

void ModbusSlave::parseADU(const ADU &adu)
{
    if (!m_devices.count(adu.addr))
        return;
    
    ModbusDevice *dev = m_devices[adu.addr];
    FunctionCode func = static_cast<FunctionCode>(adu.func);
//    uint8_t size = adu.size; // not used
    const uint8_t *data = reinterpret_cast<const uint8_t*>(adu.data);
    uint8_t *outData;
    
    m_outData.clear();
    ExceptionCode error = eNone;
    uint16_t addr, cnt, value;
    uint8_t N;

    switch (func)
    {
      case cmdReadCoils:
        addr = readWord(data);
        cnt = readWord(data);
        N = (cnt + 7) / 8;
        m_outData.resize(N + 1);
        m_outData[0] = N;
        error = dev->readCoils(addr, cnt, (uint8_t*)m_outData.data() + 1);
        break;

      case cmdReadDiscreteInputs:
        addr = readWord(data);
        cnt = readWord(data);
        N = (cnt + 7) / 8;
        m_outData.resize(N + 1);
        m_outData[0] = N;
        error = dev->readDiscreteInputs(addr, cnt, (uint8_t*)m_outData.data() + 1);
        break;

      case cmdReadHoldingRegisters:
        addr = readWord(data);
        cnt = readWord(data);
        N = cnt * 2;
        m_outData.resize(N + 1);
        m_outData[0] = N;
        error = dev->readHoldingRegisters(addr, cnt, (uint8_t*)(m_outData.data() + 1));
        break;

      case cmdReadInputRegisters:
        addr = readWord(data);
        cnt = readWord(data);
        N = cnt * 2;
        m_outData.resize(N + 1);
        m_outData[0] = N;
        error = dev->readInputRegisters(addr, cnt, (uint8_t*)(m_outData.data() + 1));
        break;

      case cmdWriteSingleCoil:
        addr = readWord(data);
        value = readWord(data);
        error = dev->writeSingleCoil(addr, value);
        m_outData.resize(4);
        outData = (uint8_t*)m_outData.data();
        writeWord(outData, addr);
        writeWord(outData, value);
        break;

      case cmdWriteSingleRegister:
        addr = readWord(data);
        value = readWord(data);
        error = dev->writeSingleRegister(addr, value);
        m_outData.resize(4);
        outData = (uint8_t*)m_outData.data();
        writeWord(outData, addr);
        writeWord(outData, value);
        break;

      case cmdReadExceptionStatus:
        error = eIllegalFunction;
        break;

      case cmdDiagnostics:
        error = eIllegalFunction;
        break;

      case cmdGetCommEventCounter:
        error = eIllegalFunction;
        break;

      case cmdGetCommEventLog:
        error = eIllegalFunction;
        break;

      case cmdWriteMultipleCoils:
        addr = readWord(data);
        cnt = readWord(data);
        N = *data++;
        if (N == (cnt + 7) / 8)
            error = dev->writeMultipleCoils(addr, cnt, data);
        else
            error = eIllegalDataValue;
        m_outData.resize(4);
        outData = (uint8_t*)m_outData.data();
        writeWord(outData, addr);
        writeWord(outData, cnt);
        break;

      case cmdWriteMultipleRegisters:
        addr = readWord(data);
        cnt = readWord(data);
        N = *data++;
        if (N == cnt * 2)
            error = dev->writeMultipleRegisters(addr, cnt, data);
        else
            error = eIllegalDataValue;
        m_outData.resize(4);
        outData = (uint8_t*)m_outData.data();
        writeWord(outData, addr);
        writeWord(outData, cnt);
        break;

      case cmdReportSlaveId:
        error = eIllegalFunction;
        break;

      case cmdReadFileRecord:
        error = eIllegalFunction;
        break;

      case cmdWriteFileRecord:
        error = eIllegalFunction;
        break;

      case cmdMaskWriteRegister:
        error = eIllegalFunction;
        break;

      case cmdReadWriteMultipleRegisters:
        error = eIllegalFunction;
        break;

      case cmdReadFifoQueue:
        error = eIllegalFunction;
        break;

      default:
        error = eIllegalFunction;
    }

    if (error != eNone)
    {
        sendException(dev->address(), error, func);
    }
    else
    {
        ADU adu;
        adu.addr = dev->address();
        adu.func = func;
        adu.data = m_outData.data();
        adu.size = m_outData.size();
        m_transport->writeADU(adu);
    }
}

void ModbusSlave::sendException(uint8_t addr, ExceptionCode e, FunctionCode f)
{
    ADU adu;
    adu.addr = addr;
    adu.func = cmdException | f;
    adu.data = reinterpret_cast<const char*>(&e);
    adu.size = 1;
    m_transport->writeADU(adu);
}
