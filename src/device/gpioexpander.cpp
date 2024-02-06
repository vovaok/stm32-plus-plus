#include "gpioexpander.h"
#include "core/application.h"

GpioExpander::GpioExpander(Gpio::PinName sck, Gpio::PinName data, Gpio::PinName sl, Gpio::PinName cs) :
    ShiftRegister(sck, data, sl, cs)
{
    memset(&m_gpio, 0, sizeof(GPIO_TypeDef));
    stmApp()->registerTaskEvent(EVENT(&GpioExpander::task));
    m_read = true;
    task(); // update registers
}

GpioExpander::GpioExpander(Gpio* sck, Gpio* data, Gpio* sl, Gpio* cs) :
    ShiftRegister(sck, data, sl, cs)
{
    memset(&m_gpio, 0, sizeof(GPIO_TypeDef));
    stmApp()->registerTaskEvent(EVENT(&GpioExpander::task));
    m_read = true;
    task(); // update registers
}

GpioExpander::GpioExpander(Gpio::PinName sck, Gpio::PinName data, Gpio::PinName cs) :
    ShiftRegister(sck, Gpio::noPin, data, cs)
{
    memset(&m_gpio, 0, sizeof(GPIO_TypeDef));
    stmApp()->registerTaskEvent(EVENT(&GpioExpander::task));
    m_write = true;
    m_gpio.MODER = 0x55555555; // all outputs (if someone wants)
    task(); // update registers
}

GpioExpander::GpioExpander(Gpio* sck, Gpio* data, Gpio* cs) :
    ShiftRegister(sck, new Gpio(Gpio::noPin), data, cs)
{
    memset(&m_gpio, 0, sizeof(GPIO_TypeDef));
    stmApp()->registerTaskEvent(EVENT(&GpioExpander::task));
    m_write = true;
    m_gpio.MODER = 0x55555555; // all outputs (if someone wants)
    task(); // update registers
}

void GpioExpander::task()
{    
    if (m_read)
    {
        m_gpio.IDR = read(m_bits);
    }
    if (m_write)
    {
        uint32_t bsrr = m_gpio.BSRR;
        m_gpio.BSRR = 0;
        m_gpio.ODR &= ~(bsrr >> 16);
        m_gpio.ODR |= bsrr & 0xFFFF;
        
        write(m_gpio.ODR, m_bits);
        
        m_gpio.IDR = m_gpio.ODR;
    }
}