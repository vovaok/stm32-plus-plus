#include "fdcan.h"
#include "rcc.h"

FdCan::FdCan(Gpio::Config fdcanRx, Gpio::Config fdcanTx)
{
    if (GpioConfigGetPeriphNumber(fdcanRx) !=
        GpioConfigGetPeriphNumber(fdcanTx))
        THROW(Exception::InvalidPin);
    
    Gpio::config(fdcanRx);
    Gpio::config(fdcanTx);
    
    switch (GpioConfigGetPeriphNumber(fdcanRx))
    {
    case 1:
        m_dev = FDCAN1;
        break;
        
    case 2:
        m_dev = FDCAN2;
        break;
        
    case 3:
        m_dev = FDCAN3;
        break;
    }
    
    rcc().setPeriphEnabled(m_dev);
}