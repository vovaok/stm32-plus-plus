#include "led.h"

Led::Led(char portLetter, int pinNumber) :
  mState(false),mCountMig(0)
{
    if (pinNumber<0 || pinNumber>31)
    {
        mPort = 0L; 
//        #ifndef NO_EXCEPTIONS
        throw Exception::invalidPin;
//        #else
//        while(1);
//        #endif
    }
    
    mPin = 1 << pinNumber;
    
    uint32_t gpioRcc;
    
    switch (portLetter)
    {
      case 'a': case 'A': 
        mPort = GPIOA;
        gpioRcc = RCC_AHB1Periph_GPIOA;
        break;
        
      case 'b': case 'B': 
        mPort = GPIOB;
        gpioRcc = RCC_AHB1Periph_GPIOB;
        break;
        
      case 'c': case 'C': 
        mPort = GPIOC;
        gpioRcc = RCC_AHB1Periph_GPIOC;
        break;
        
      case 'd': case 'D': 
        mPort = GPIOD;
        gpioRcc = RCC_AHB1Periph_GPIOD;
        break;
        
      case 'e': case 'E': 
        mPort = GPIOE;
        gpioRcc = RCC_AHB1Periph_GPIOE;
        break;
        
      case 'f': case 'F': 
        mPort = GPIOF;
        gpioRcc = RCC_AHB1Periph_GPIOF;
        break;
        
      case 'g': case 'G': 
        mPort = GPIOG;
        gpioRcc = RCC_AHB1Periph_GPIOG;
        break;
        
      case 'h': case 'H': 
        mPort = GPIOH;
        gpioRcc = RCC_AHB1Periph_GPIOH;
        break;
        
      case 'i': case 'I': 
        mPort = GPIOI;
        gpioRcc = RCC_AHB1Periph_GPIOI;
        break;
        
      default:
//        #ifndef NO_EXCEPTIONS
        throw Exception::invalidPort;
//        #else
//        while(1);
//        #endif
    }
     
    RCC_AHB1PeriphClockCmd(gpioRcc, ENABLE); 
    
    GPIO_InitTypeDef GPIO_InitStructure;  
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = mPin; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(mPort, &GPIO_InitStructure);
}
//---------------------------------------------------------------------------

void Led::on()
{
    mState = true;
    GPIO_SetBits(mPort, mPin);
}

void Led::off()
{
    mState = false;
    GPIO_ResetBits(mPort, mPin);
}

void Led::toggleSkip(char count)
{
    mCountMig++;
    if (mCountMig>=count)
    {
        mState = !mState;
        GPIO_ToggleBits(mPort, mPin);
        mCountMig=0;
    }
}

void Led::toggleSkip(char count1, char count2)
{
    mCountMig++;
    if (mCountMig >= count1 + count2)
    {
        on();
        mCountMig=0;
    }
    else if (mCountMig>=count1)
    {
        off();
    }
}

void Led::toggle()
{
    mState = !mState;
    GPIO_ToggleBits(mPort, mPin);
}
//---------------------------------------------------------------------------

void Led::setState(bool newState)
{
    mState = newState;
    GPIO_WriteBit(mPort, mPin, newState? Bit_SET: Bit_RESET);
}
//---------------------------------------------------------------------------
