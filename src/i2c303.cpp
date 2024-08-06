#include "i2c303.h"
#include <stdio.h>
#include "core/coreexception.h"

#define I2C_TIMEOUT 5000

I2c::I2c(Gpio::Config pinSDA, Gpio::Config pinSCL)
{
//    failAddress = 0;

    int i2cNumber = GpioConfigGetPeriphNumber(pinSDA);
    if (i2cNumber != GpioConfigGetPeriphNumber(pinSCL))
        THROW(Exception::InvalidPeriph);

    Gpio::config(pinSDA);
    Gpio::config(pinSCL);

    switch (i2cNumber)
    {
    case 1:
        m_dev = I2C1;
        RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
        break;
#if defined (I2C2)
    case 2:
        m_dev = I2C2;
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
        break;
#endif
#if defined (I2C3)
    case 3:
        m_dev = I2C3;
        RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
        break;
#endif
    }
}

void I2c::setBusClock(int clk_Hz)   //fake settings
{
  m_dev->TIMINGR = (uint32_t)0x2000090E;    //100кГц

}

void I2c::setAddress(uint8_t address)
{
    m_dev->OAR1 = address;
}

void I2c::open()
{
    m_dev->CR1 |= I2C_CR1_PE; 
}

void I2c::close()
{
    m_dev->CR1 &= ~I2C_CR1_PE;
}
//---------------------------------------------------------------------------

bool I2c::read(uint8_t address, uint8_t *data, uint16_t size)
{
   
    return 0;
}

bool I2c::write(uint8_t address, uint8_t *data, uint16_t size)
{
   
    return 0;
}



bool I2c::readReg(uint8_t hw_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size)
{
  
  
     m_dev->CR2 = (hw_addr ) | I2C_CR2_START   | (2 << 16);
     
    writeData(reg_addr>>8);
    writeData(reg_addr&0xff);
     
      // Ждем окончания передачи
    while (!(m_dev->ISR & I2C_ISR_TC));
    
  // Шлем стартовый сигнал и адрес устройства на чтение
    m_dev->CR2 = (hw_addr) | I2C_CR2_RD_WRN; // 7-битный адрес, чтение (R/W = 1)
    m_dev->CR2 |= (size << 16); // Количество читаемых байт
    m_dev->CR2 |= I2C_CR2_START; // Старт

    // Ожидаем приема байта
     while (size--)
     {
       readData(buffer++);
     }

    // Генерация стоп-сигнала
    m_dev->CR2 |= I2C_CR2_STOP;
  
}



bool I2c::writeReg(uint8_t hw_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size)
{
  
    m_dev->CR2 = (hw_addr ); // 7-битный адрес, запись (R/W = 0)
    m_dev->CR2 |=  ((size + 2) << 16); // Количество передаваемых байт (2 байта памяти + 1 байт данных)
    m_dev->CR2 |= I2C_CR2_START; // Старт

    // Ожидаем завершения передачи адреса и данных
    
     writeData(reg_addr>>8);
     writeData(reg_addr&0xff);
    
     while (size--)
     {
       writeData(*buffer++);
     }
    // Ждем завершения передачи
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Генерация стоп-сигнала
    m_dev->CR2 |= I2C_CR2_STOP;

    // Ожидаем, пока стоп-сигнал не будет завершен
    while (m_dev->ISR & I2C_ISR_STOPF);
    return 1;
  
}
//---------------------------------------------------------------------------





bool I2c::writeData(uint8_t data)
{
    int timeout = I2C_TIMEOUT;
  
    while (!(m_dev->ISR & I2C_ISR_TXIS) && timeout)
      timeout --;
      m_dev->TXDR = data; 
    
    return timeout;
}

bool I2c::readData(uint8_t *buf)
{
    int timeout = I2C_TIMEOUT * 10;
    while (!(m_dev->ISR & I2C_ISR_RXNE) && timeout)
      timeout --;
     *buf = m_dev->RXDR;
   
    return timeout;
}


