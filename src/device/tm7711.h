#pragma once

#include "gpio.h"

class TM7711
{
public:
    TM7711(Gpio::PinName sck, Gpio::PinName out);
    
    int read();
    const int &value() const {return m_value;};
    
private:
    Gpio *m_sck;
    Gpio *m_out;
    int m_value = 0;
    
    void delay();
};
