#include "i2c.h"
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

    case 2:
        m_dev = I2C2;
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
        break;

    case 3:
        m_dev = I2C3;
        RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
        break;
    }
}

void I2c::setBusClock(int clk_Hz)
{
    int pclk1 = rcc().pClk1();
    uint16_t freqrange = pclk1 / 1000000;
    // prescaler config:
    m_dev->CR2 = (m_dev->CR2 & ~I2C_CR2_FREQ) | (freqrange & I2C_CR2_FREQ);

    // disable I2C periph
    m_dev->CR1 &= ~I2C_CR1_PE;
    uint16_t ccr = 0;

    if (clk_Hz <= 100000)
    {
        ccr = pclk1 / (clk_Hz << 1);
        if (ccr < 4)
            ccr = 4;
        m_dev->TRISE = freqrange + 1;
    }
    else
    {
        if (clk_Hz < 400000) // I2C_DutyCycle_2
        {
            ccr = pclk1 / (clk_Hz * 3);
        }
        else // if clk_Hz == 400000 // I2C_DutyCycle_16_9
        {
            ccr = pclk1 / (clk_Hz * 25);
            ccr |= I2C_CCR_DUTY;
        }

        if ((ccr & I2C_CCR_CCR) == 0)
            ccr |= 0x0001;

        ccr |= I2C_CCR_FS; // fast mode
        m_dev->TRISE = freqrange * 300 / 1000 + 1;
    }
    m_dev->CCR = ccr;

}

void I2c::setAddress(uint8_t address)
{
    m_dev->OAR1 = address;
}

void I2c::open()
{
    m_dev->CR1 |= I2C_CR1_PE;
    m_dev->CR1 |= I2C_CR1_ACK; // enable ACK
}

void I2c::close()
{
    m_dev->CR1 &= ~I2C_CR1_PE;
}
//---------------------------------------------------------------------------

bool I2c::read(uint8_t address, uint8_t *data, uint16_t size)
{
    if (!size)
        return false;

    bool r;
    r = startTransmission(DirectionReceiver, address);
    if (r)
    {
        setAcknowledge(true);
        while (--size)
        {
            r &= readData(data);
            data++;
        }
        setAcknowledge(false);
            r &= readData(data);
    }
    if (r)
        r = stopTransmission();
    return r;
}

bool I2c::write(uint8_t address, uint8_t *data, uint16_t size)
{
    if (!size)
        return false;

    bool r;
    r = startTransmission(DirectionTransmitter, address);
    if (r)
    {
        setAcknowledge(false);
        do
            r &= writeData(*data++);
        while (--size);
    }
    if (r)
        r = stopTransmission();
    return r;
}

//bool I2c::regRead(unsigned char address, unsigned char index, unsigned char *buffer)
//{
//    bool success;
//    success = startTransmission(DirectionTransmitter, address);
//    if (success)
//        success = writeData(index);
//    if (success)
//        success = startTransmission(DirectionReceiver, address);
//    if (success)
//    {
//        setAcknowledge(false);
//        success = readData(buffer);
//    }
//    success = stopTransmission();
//    return success;
//}

bool I2c::readReg(uint8_t hw_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size)
{
    bool success;
    if (!size)
        return false;

    success = startTransmission(DirectionTransmitter, hw_addr);
    if (success)
        success = writeData(reg_addr);
    if (success)
        success = startTransmission(DirectionReceiver, hw_addr);
    if (success)
    {
        setAcknowledge(true);
        while (size--)
        {
            if (!size)
                setAcknowledge(false);
            success = success && readData(buffer);
            buffer++;
        }
    }
    if (success)
        success = stopTransmission();
    return success;
}

//bool I2c::regWrite(unsigned char address, unsigned char index, unsigned char byte)
//{
//    bool success;
//    success = startTransmission(DirectionTransmitter, address);
//    if (success)
//        success = writeData(index);
//    if (success)
//    {
//        setAcknowledge(false);
//        success = writeData(byte);
//    }
//    if (success)
//        success = stopTransmission();
//    return success;
//}

bool I2c::writeReg(uint8_t hw_addr, uint8_t reg_addr, const uint8_t *buffer, uint8_t size)
{
    bool success;
    success = startTransmission(DirectionTransmitter, hw_addr);
    if (success)
        success = writeData(reg_addr);
    if (success)
    {
        setAcknowledge(false);
        while (size--)
            success = success && writeData(*buffer++);
    }
    if (success)
        success = stopTransmission();
    return success;
}
//---------------------------------------------------------------------------

bool I2c::checkEvent(Event e)
{
    union
    {
        uint32_t f;
        uint16_t r[2];
    };
    r[0] = m_dev->SR1;
    r[1] = m_dev->SR2;

    return (uint32_t)e == (f & (uint32_t)e);
}

bool I2c::startTransmission(Direction dir, uint8_t slaveAddress)
{
    lastAddress = slaveAddress;

    int timeout = I2C_TIMEOUT;

    // если не мастер:
//    if (!(m_dev->SR2 & I2C_SR2_MSL))
//    {
//        // На всякий случай ждем, пока шина осовободится
//        while (I2C_GetFlagStatus(m_dev, I2C_FLAG_BUSY) && timeout)
//            --timeout;
//    }

    if (timeout)
    {
        // Генерируем старт
        m_dev->CR1 |= I2C_CR1_START;
        timeout = I2C_TIMEOUT;
        // Ждем пока взлетит нужный флаг
        while (!checkEvent(EventMasterModeSelect) && timeout)
            --timeout;
    }

    if (!timeout)
    {
        m_dev->CR1 &= ~I2C_CR1_START;
        printf("i2c START failed\n");
    }
    else
    {
        // Посылаем адрес подчиненному
        m_dev->DR = slaveAddress | dir;

        timeout = I2C_TIMEOUT;
        // А теперь у нас два варианта развития событий - в зависимости от выбранного направления обмена данными
        if (dir == DirectionTransmitter)
        {
            while (!checkEvent(EventMasterTransmitterModeSelected) && timeout)
                --timeout;
        }
        else if (dir == DirectionReceiver)
        {
            while (!checkEvent(EventMasterReceiverModeSelected) && timeout)
                --timeout;
        }
    }

    return timeout;
}

bool I2c::stopTransmission()
{
    int timeout = I2C_TIMEOUT;
    m_dev->CR1 |= I2C_CR1_STOP;
    while ((m_dev->SR2 & I2C_SR2_MSL) && timeout)
        --timeout;
    if (!timeout)
    {
        m_dev->CR1 &= ~I2C_CR1_STOP;
//        failAddress = lastAddress;
        printf("i2c STOP failed\n");
//        init();
        close();
        open();
        setAcknowledge(false);
    }
    return timeout;
}

bool I2c::writeData(uint8_t data)
{
    int timeout = I2C_TIMEOUT;
    // Просто выставляем данные и ждем, пока они улетят
    m_dev->DR = data;
    while (!checkEvent(EventMasterByteTransmitted) && timeout)
        --timeout;
    if (!timeout)
    {
        printf("i2c WRITE failed\n");
    }
    return timeout;
}

bool I2c::readData(uint8_t *buf)
{
    int timeout = I2C_TIMEOUT * 10;
    // Тут картина похожа, как только данные пришли быстренько считываем их и возвращаем
    while (!checkEvent(EventMasterByteReceived) && timeout)
        --timeout;
    if (timeout)
        *buf = m_dev->DR;
    if (!timeout)
    {
        printf("i2c READ failed\n");
    }
    return timeout;
}

void I2c::setAcknowledge(bool state)
{
    if (state)
        m_dev->CR1 |= I2C_CR1_ACK;
    else
        m_dev->CR1 &= ~I2C_CR1_ACK;
}
