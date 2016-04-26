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
   
    I2C_InitTypeDef i2c;
   
    i2c.I2C_ClockSpeed = mClockSpeed; 
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
   
    i2c.I2C_OwnAddress1 = mAddress;
    i2c.I2C_Ack = I2C_Ack_Disable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(mI2c, &i2c);
 
    
    I2C_Cmd(mI2c, ENABLE);
}


void I2c::startTransmission(uint8_t transmissionDirection,  uint8_t slaveAddress)
{
    // На всякий слуыай ждем, пока шина осовободится
    while(I2C_GetFlagStatus(mI2c, I2C_FLAG_BUSY));
 
    // Генерируем старт
    I2C_GenerateSTART(mI2c, ENABLE);
 
    // Ждем пока взлетит нужный флаг
    while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_MODE_SELECT));
 
    // Посылаем адрес подчиненному
    I2C_Send7bitAddress(mI2c, slaveAddress, transmissionDirection);
 
    // А теперь у нас два варианта развития событий - в зависимости от выбранного направления обмена данными
    if(transmissionDirection== I2C_Direction_Transmitter)
    {
    	while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    }
 
    if(transmissionDirection== I2C_Direction_Receiver)
    {
	while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    }
}

void I2c::writeData(uint8_t data)
{
    // Просто вызываем готоваую функцию из SPL и ждем, пока данные улетят
    I2C_SendData(mI2c, data);
    while(!I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

void I2c::stopTransmission ()
 {
   I2C_GenerateSTOP(mI2c, ENABLE);
 }

uint8_t I2c::readData()
{
    // Тут картина похожа, как только данные пришли быстренько считываем их и возвращаем
    while( !I2C_CheckEvent(mI2c, I2C_EVENT_MASTER_BYTE_RECEIVED) );
  uint8_t   data = I2C_ReceiveData(mI2c);
    return data;
}