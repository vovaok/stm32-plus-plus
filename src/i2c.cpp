#include "i2c.h"

I2c::I2c(int i2cNumber, int clockSpeed, int address, Gpio::Config pinSDA, Gpio::Config pinSCL) 
{
    
    mAddress = address;
    mClockSpeed = clockSpeed;
    
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
    
    
    // I2C_SoftwareResetCmd  (mI2c, ENABLE);
    // for(int i=0;i<100000;i++);
    //I2C_SoftwareResetCmd  (mI2c, DISABLE);
    
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


void I2c::startTransmission(uint8_t transmissionDirection,  uint8_t slaveAddress)
{
    // На всякий слуыай ждем, пока шина осовободится
    //  while(I2C_GetFlagStatus(mI2c, I2C_FLAG_BUSY));
    
    // Генерируем старт
    mI2c->CR1 |= I2C_CR1_START;
    
    int delay=0;
    
    // Ждем пока взлетит нужный флаг
    while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_MODE_SELECT)&&delay<5000)
    {
      delay++;
    }
    
    
    
    
    // Посылаем адрес подчиненному
    I2C_Send7bitAddress(mI2c, slaveAddress, transmissionDirection);
    
    //    mI2c->DR = slaveAddress;
    delay = 0;
    // А теперь у нас два варианта развития событий - в зависимости от выбранного направления обмена данными
    if(transmissionDirection== I2C_Direction_Transmitter)
    {
        
        while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)&&delay<5000)
        {
            delay++;
        }
        
        delay =0;
    }
    
    else if(transmissionDirection== I2C_Direction_Receiver)
    {
        
        
        
        while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)&&delay<5000)
        {
          delay++;
        }
    }
}

void I2c::writeData(uint8_t data)
{
  
  int delay =0;
    // Просто вызываем готоваую функцию из SPL и ждем, пока данные улетят
    I2C_SendData(mI2c, data);
    while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&&delay<5000)
    {
      delay++;
    }
}

void I2c::stopTransmission ()
{
    I2C_GenerateSTOP(mI2c, ENABLE);
}

uint8_t I2c::readData()
{
  int delay=0;
    // Тут картина похожа, как только данные пришли быстренько считываем их и возвращаем
    while( !I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_BYTE_RECEIVED)&&delay<5000 )
    {
      delay++;
    }
    uint8_t   data = I2C_ReceiveData(mI2c);
    return data;
}