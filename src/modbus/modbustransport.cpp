#include "modbustransport.h"

void ModbusTransport::receiveADU(const Modbus::ADU &adu)
{
//    m_adu = adu;
    if (onAduReceived)
        onAduReceived(adu);
}