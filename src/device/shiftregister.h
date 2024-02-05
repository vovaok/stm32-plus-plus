#pragma once 

#include "gpio.h"

class ShiftRegister
{
public:
    ShiftRegister(Gpio::PinName sck, Gpio::PinName miso, Gpio::PinName mosi, Gpio::PinName cs);
    ShiftRegister(Gpio* sck, Gpio* miso, Gpio* mosi, Gpio* cs);
    
    uint32_t read(int bits);
    void write(uint32_t data, int bits);
    
private:
    Gpio *m_sck = nullptr;
    Gpio *m_miso = nullptr;
    Gpio *m_mosi = nullptr;
    Gpio *m_cs = nullptr;    
};