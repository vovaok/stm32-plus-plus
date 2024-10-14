#ifndef _MODBUS_H
#define _MODBUS_H

#include "core/device.h"

namespace Modbus
{
    
struct ADU
{
    uint8_t addr;
    uint8_t func;
    uint8_t size;
    const char *data;
};

enum FunctionCode
{
    cmdNone                         = 0x00,
    cmdReadCoils                    = 0x01,
    cmdReadDiscreteInputs           = 0x02,
    cmdReadHoldingRegisters         = 0x03,
    cmdReadInputRegisters           = 0x04,
    cmdWriteSingleCoil              = 0x05,
    cmdWriteSingleRegister          = 0x06,
    cmdReadExceptionStatus          = 0x07,
    cmdDiagnostics                  = 0x08,
    cmdGetCommEventCounter          = 0x0B,
    cmdGetCommEventLog              = 0x0C,
    cmdWriteMultipleCoils           = 0x0F,
    cmdWriteMultipleRegisters       = 0x10,
    cmdReportSlaveId                = 0x11,
    cmdReadFileRecord               = 0x14,
    cmdWriteFileRecord              = 0x15,
    cmdMaskWriteRegister            = 0x16,
    cmdReadWriteMultipleRegisters   = 0x17,
    cmdReadFifoQueue                = 0x18,
    cmdException                    = 0x80
};

enum ExceptionCode
{
    eNone               = 0,
    eIllegalFunction    = 1,
    eIllegalDataAddress = 2,
    eIllegalDataValue   = 3,
    eSlaveDeviceFailure = 4,
    eAcknowledge        = 5,
    eSlaveDeviceBusy    = 6,
    eNegativeAcknowledge= 7,
    eMemoryParityError  = 8,
};

extern uint16_t readWord(uint8_t const * &data);
extern void writeWord(uint8_t *&data, uint16_t word);
uint16_t crc16(uint8_t *data, int size);
    
}

#endif