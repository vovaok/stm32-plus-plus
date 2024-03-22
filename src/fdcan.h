#ifndef _FDCAN_H
#define _FDCAN_H

#include "gpio.h"

class FdCan
{
public:
    FdCan(Gpio::Config fdcanRx, Gpio::Config fdcanTx);
    
private:
    FDCAN_GlobalTypeDef *m_dev = nullptr;
    
};

#endif