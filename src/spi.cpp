#include "spi.h"

Spi::Spi(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct) : countCs(24)
{
  
   for(int i=0;i<24;i++)
     zero[i] =0;
  
   zero[0] = 5.74322; zero[1] = 1.49103; zero[2] = 3.7061;
   zero[3] = 3.26431; zero[4] = 5.41188-4.33809; 
   
   zero[7] = 2.98206; zero[8] = 4.57083; zero[9] = 1.97577;
   zero[10] = 5.72482; zero[11] = 5.77199;
  
   
   for(int i=0;i<countCs;i++)
   {
   cs[i] = new Gpio((Gpio::PinName)(Gpio::PD0+i));
   cs[i]->setAsOutput();
   cs[i]->write(1);
   }
  if(SPIx==SPI1)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    Gpio::config(Gpio::SPI1_MOSI_PA7);
    Gpio::config(Gpio::SPI1_SCK_PA5);
  
    
    
  }
  else if(SPIx==SPI2)
  {
    RCC_APB1PeriphClockCmd  ( RCC_APB1Periph_SPI2,ENABLE);
  

    Gpio::config(Gpio::SPI2_MISO_PB14 );
    Gpio::config(Gpio::SPI2_SCK_PB13);
    Gpio::config(Gpio::SPI2_MOSI_PB15);
    }

  SPI_Init(SPIx,SPI_InitStruct);
  SPI_Cmd (SPIx,ENABLE);
  
}

float* Spi::spiRead() //unsigned short* Spi::spiRead()
{
  unsigned short temp[24];
  for(int i=0;i<countCs;i++)
  {
    cs[i]->write(0);
 
    temp[i] = 0xffff;
    SPI_I2S_SendData  (SPI2, temp[i]); 
  
    while(!SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE));
  
    while(!SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE));
    cs[i]->write(1);
    temp[i] =  SPI_I2S_ReceiveData  ( SPI2 ); // read data from SPI register
 
    temp[i] &=~63; 
    
    u32result[i] = (float)(temp[i]*2*PI)/65536;
    u32result[i]-= zero[i];
       
  }
 return u32result;


}