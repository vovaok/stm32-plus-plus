#include "modbus.h"
#include "core/core.h"

namespace Modbus
{

uint16_t readWord(uint8_t const * &data)
{
    uint16_t w;
    reinterpret_cast<uint8_t*>(&w)[1] = *data++;
    reinterpret_cast<uint8_t*>(&w)[0] = *data++;
    return w;
}

void writeWord(uint8_t *&data, uint16_t word)
{
    *data++ = word >> 8;
    *data++ = (uint8_t)word;
}

uint16_t crc16(uint8_t *data, int size)
{
    uint16_t crc = 0xFFFF;
    for (int i=0; i<size; i++)
    {
        crc ^= (uint16_t)data[i];              // XOR byte into least sig. byte of crc
        for (int j=8; j; j--)
        {                                   // Loop over each bit
            if (crc & 0x0001)
            {                               // If the LSB is set
                crc >>= 1;                  // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else                            // Else LSB is not set
            {
                crc >>= 1;                  // Just shift right
            }
        }
    }
    return crc;
}

}
