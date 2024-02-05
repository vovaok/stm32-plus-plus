#pragma once

#include "shiftregister.h"

class GpioExpander : public ShiftRegister
{
public:
    GpioExpander(Gpio::PinName sck, Gpio::PinName data, Gpio::PinName sl, Gpio::PinName cs);
    GpioExpander(Gpio* sck, Gpio* data, Gpio* sl, Gpio* cs);
    GpioExpander(Gpio::PinName sck, Gpio::PinName data, Gpio::PinName cs);
    GpioExpander(Gpio* sck, Gpio* data, Gpio* cs);
    
    void setDataBits(int bits) {m_bits = bits;}
    
    GPIO_TypeDef *gpio() {return &m_gpio;}
    
private:
    GPIO_TypeDef m_gpio;
    int m_bits = 16;
    bool m_read = false;
    bool m_write = false;
    
    void task();
};
