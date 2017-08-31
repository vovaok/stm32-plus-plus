#include "i2c.h"

I2c::I2c(int i2cNumber, int clockSpeed, int address, Gpio::Config pinSDA, Gpio::Config pinSCL) 
{
    
    mAddress = address;
    mClockSpeed = clockSpeed;
    failAddress = 0;
    
    switch (i2cNumber)
    {
    
    case 1:
        mI2c = I2C1;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        break;
        
    case 2:
        mI2c = I2C2;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        break;
        
    case 3:
        mI2c = I2C3;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
        break;
        
    }
    Gpio::config(pinSDA);
    Gpio::config(pinSCL);
    init();
}


void I2c::init(void)
{
    I2C_DeInit(mI2c);
    
    
    I2C_SoftwareResetCmd  (mI2c, ENABLE);
    // for(int i=0;i<100000;i++);
    I2C_SoftwareResetCmd  (mI2c, DISABLE);
    
    I2C_InitTypeDef i2c;
    
    i2c.I2C_ClockSpeed = mClockSpeed; 
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_16_9;
    
    i2c.I2C_OwnAddress1 = mAddress;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    
    
    I2C_Cmd(mI2c, ENABLE);
    I2C_Init(mI2c, &i2c);
    
}


bool I2c::startTransmission(uint8_t transmissionDirection, uint8_t slaveAddress)
{
    lastAddress = slaveAddress;
    
    int timeout = 500;
    
    // если не мастер:
//    if (!(mI2c->SR2 & I2C_SR2_MSL))
//    {
//        // На всякий случай ждем, пока шина осовободится
//        while (I2C_GetFlagStatus(mI2c, I2C_FLAG_BUSY) && timeout)
//            --timeout;
//    }
    
    if (timeout)
    {
        // Генерируем старт
        mI2c->CR1 |= I2C_CR1_START;
        timeout = 500;
        // Ждем пока взлетит нужный флаг
        while (!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_MODE_SELECT) && timeout)
            --timeout;
    }
    
    if (!timeout)
    {
        mI2c->CR1 &= ~I2C_CR1_START;
        printf("i2c START failed\n");
    }
    else
    {
        // Посылаем адрес подчиненному
        I2C_Send7bitAddress(mI2c, slaveAddress, transmissionDirection);
        
        timeout = 500;
        // А теперь у нас два варианта развития событий - в зависимости от выбранного направления обмена данными
        if (transmissionDirection == I2C_Direction_Transmitter)
        {
            while (!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && timeout)
                --timeout;
        }
        else if (transmissionDirection == I2C_Direction_Receiver)
        {
            while (!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && timeout)
                --timeout;
        }
    }
    
    return timeout;
}

bool I2c::stopTransmission()
{
    int timeout = 500;
    mI2c->CR1 |= I2C_CR1_STOP;
    while ((mI2c->SR2 & I2C_SR2_MSL) && timeout)
        --timeout;
    if (!timeout)
    {
        mI2c->CR1 &= ~I2C_CR1_STOP;
        failAddress = lastAddress;
        printf("i2c STOP failed\n");
        init();
        setAcknowledge(false);
    }
    return timeout;
}

bool I2c::writeData(uint8_t data)
{
    int timeout = 500;
    // Просто вызываем готовую функцию из SPL и ждем, пока данные улетят
    I2C_SendData(mI2c, data);
    while (!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && timeout)
        --timeout;
    if (!timeout)
    {
        printf("i2c WRITE failed\n");
    }
    return timeout;
}

bool I2c::readData(unsigned char *buf)
{
    int timeout = 5000;
    // Тут картина похожа, как только данные пришли быстренько считываем их и возвращаем
    while (!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_BYTE_RECEIVED) && timeout)
        --timeout;
    if (timeout)
        *buf = I2C_ReceiveData(mI2c);
    if (!timeout)
    {
        printf("i2c READ failed\n");
    }
    return timeout;
}

void I2c::setAcknowledge(bool state)
{
    if (state)
        mI2c->CR1 |= I2C_CR1_ACK;
    else
        mI2c->CR1 &= ~I2C_CR1_ACK;
}
//---------------------------------------------------------------------------

bool I2c::regRead(unsigned char address, unsigned char index, unsigned char *buffer)
{
    bool success;
    success = startTransmission(I2C_Direction_Transmitter, address);
    if (success)
        success = writeData(index);
    if (success)
        success = startTransmission(I2C_Direction_Receiver, address);
    if (success)
    {
        setAcknowledge(false);
        success = readData(buffer);
    }
    success = stopTransmission();
    return success;
}

bool I2c::multipleRead(unsigned char address, unsigned char index, unsigned char *buffer, unsigned char length)
{
    bool success;
    success = startTransmission(I2C_Direction_Transmitter, address);
    if (success)
        success = writeData(index);
    if (success)
        success = startTransmission(I2C_Direction_Receiver, address);
    if (success)
    {
        setAcknowledge(true);
        for (int i=0; i<length; i++)
        {
            if (i == length-1)
                setAcknowledge(false); 
            success = success && readData(buffer + i);
        }
    }
    if (success)
        success = stopTransmission();
    return success;
}

bool I2c::regWrite(unsigned char address, unsigned char index, unsigned char byte)
{
    bool success;
    success = startTransmission(I2C_Direction_Transmitter, address);
    if (success)
        success = writeData(index);
    if (success)
    {
        setAcknowledge(false);
        success = writeData(byte);
    }
    if (success)
        success = stopTransmission();
    return success;
}

bool I2c::multipleWrite(unsigned char address, unsigned char index, const unsigned char *buffer, unsigned char length)
{
    bool success;
    success = startTransmission(I2C_Direction_Transmitter, address);
    if (success)
        success = writeData(index);
    if (success)
    {
        setAcknowledge(false);
        for (int i=0; i<length; i++)
            success = success && writeData(buffer[i]);
    }
    if (success)
        success = stopTransmission();
    return success;
}
//---------------------------------------------------------------------------
