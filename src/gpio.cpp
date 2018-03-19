#include "gpio.h"

#if defined(STM32F37X)
#define BSRRL   BSRR
#define BSRRH   BRR
#endif

unsigned char Gpio::mPinsUsed[140]; // assuming all items = 0 at startup

Gpio::Gpio(PinName pin, Flags flags, PinAF altFunction)
{
    mConfig.pin = pin;
    mConfig.flags = flags;
    mConfig.af = altFunction;
    mPort = getPortByNumber(mConfig.portNumber);
    mPin = pin==noPin? 0: (1 << mConfig.pinNumber);
    config(mConfig.config);
}

Gpio::Gpio(Config conf)
{
    mConfig.config = conf;
    mPin = mConfig.pin==noPin? 0: (1 << mConfig.pinNumber);
    mPort = getPortByNumber(mConfig.portNumber);
    config(mConfig.config);
}

Gpio::Gpio(PortName port, unsigned short mask, Flags flags)
{
    mConfig.pin = port;
    mConfig.flags = flags;
    mConfig.manyPins = 1;
    mConfig.mask = mask;
    mPort = getPortByNumber(mConfig.portNumber);
    mPin = mask;
    config(mConfig.config);
}
//---------------------------------------------------------------------------

void Gpio::config(PinName pin, Flags flags, PinAF altFunction)
{  
    ConfigStruct c;
    c.pin = pin;
    c.flags = flags;
    c.af = altFunction;
    config(c.config);
}

void Gpio::config(const Config &conf)
{
    const ConfigStruct &c = reinterpret_cast<const ConfigStruct&>(conf);
    if (c.pin == noPin)
        return;
    
    GPIO_TypeDef *port = getPortByNumber(c.portNumber);
    if (!port)
        return;
    
    unsigned short mask;    
    
    if (!c.manyPins) // only one pin initialization
    {
        mask = 1 << c.pinNumber;
        usePin(c);
        
#if !defined(STM32F37X)
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA << c.portNumber, ENABLE); 
#else
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA << c.portNumber, ENABLE); 
#endif
        
        if (c.afNumber != afNone)
            GPIO_PinAFConfig(port, c.pinNumber, c.afNumber);
    }
    else // many pins initialization
    {        
        mask = c.mask;
        
#if !defined(STM32F37X)
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA << c.portNumber, ENABLE); 
#else
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA << c.portNumber, ENABLE); 
#endif
        
        for (int pin=0; pin<16; pin++)
        {
            if (!(mask & (1<<pin)))
                continue;
            
            ConfigStruct cc = c;
            cc.pinNumber = pin;
            usePin(cc);
            
            if (c.afNumber != afNone)
                GPIO_PinAFConfig(port, pin, c.afNumber);
        }
    }
    
    GPIO_InitTypeDef GPIO_InitStructure;  
    GPIO_InitStructure.GPIO_Pin = mask; 
    GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)c.mode;
    GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)c.speed;
    GPIO_InitStructure.GPIO_OType = (GPIOOType_TypeDef)c.outType;
    GPIO_InitStructure.GPIO_PuPd  = (GPIOPuPd_TypeDef)c.pull;
    GPIO_Init(port, &GPIO_InitStructure);
}

void Gpio::updateConfig()
{
    if (mConfig.pin == noPin)
        return;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = mPin; 
    GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)mConfig.mode;
    GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)mConfig.speed;
    GPIO_InitStructure.GPIO_OType = (GPIOOType_TypeDef)mConfig.outType;
    GPIO_InitStructure.GPIO_PuPd  = (GPIOPuPd_TypeDef)mConfig.pull;
    GPIO_Init(mPort, &GPIO_InitStructure);
}

//void Gpio::setFlags(Flags flags)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;  
//    GPIO_InitStructure.GPIO_Pin = mPin; 
//    GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)(flags & 0x3);
//    GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)((flags >> 5) & 0x3);
//    GPIO_InitStructure.GPIO_OType = (GPIOOType_TypeDef)((flags >> 2) & 0x1);
//    GPIO_InitStructure.GPIO_PuPd  = (GPIOPuPd_TypeDef)((flags >> 3) & 0x3);
//    GPIO_Init(port, &GPIO_InitStructure);
//}

void Gpio::config(int count, Config conf1, ...)
{
    va_list vl;
    va_start(vl, conf1);
    config(conf1);
    for (int i=1; i<count; i++)
    {
        config(static_cast<Config>(va_arg(vl, int)));
    }
    va_end(vl);
}
//---------------------------------------------------------------------------

void Gpio::usePin(const ConfigStruct &cfg)
{
    unsigned char pinId = static_cast<unsigned char>(cfg.pin);
    if (mPinsUsed[pinId])
        throw Exception::resourceBusy;
    mPinsUsed[pinId] = (cfg.af + 1) | ((cfg.mode + 1) << 4);
}
//---------------------------------------------------------------------------

GPIO_TypeDef *Gpio::getPortByNumber(int port)
{
    switch (port)
    {
        case 0x0: return GPIOA;
        case 0x1: return GPIOB;
        case 0x2: return GPIOC;
        case 0x3: return GPIOD;
        case 0x4: return GPIOE;
        case 0x5: return GPIOF;
#if !defined(STM32F37X)
        case 0x6: return GPIOG;
        case 0x7: return GPIOH;
        case 0x8: return GPIOI;
#endif
        default: return 0L;
    }
}
//---------------------------------------------------------------------------

void Gpio::setAsInput()
{
    mConfig.mode = modeIn;
    mConfig.pull = pullNone >> 3;
    updateConfig();
}

void Gpio::setAsInputPullUp()
{
    mConfig.mode = modeIn;
    mConfig.pull = pullUp >> 3;
    updateConfig();
}

void Gpio::setAsInputPullDown()
{
    mConfig.mode = modeIn;
    mConfig.pull = pullDown >> 3;
    updateConfig();
}

void Gpio::setAsOutput()
{
    mConfig.mode = modeOut;
    mConfig.outType = outPushPull >> 2;
    mConfig.pull = pullNone >> 3;
    updateConfig();
}

void Gpio::setAsOutputOpenDrain()
{
    mConfig.mode = modeOut;
    mConfig.outType = outOpenDrain >> 2;
    mConfig.pull = pullNone >> 3;
    updateConfig();
}
//---------------------------------------------------------------------------

bool Gpio::read() const
{
    if (mConfig.pin == noPin)
        return false;
    if (mConfig.mode == modeOut)
        return mPort->ODR & mPin;
    else
        return mPort->IDR & mPin;
}

void Gpio::write(bool value)
{
    if (mConfig.pin == noPin)
        return;
    if (value)
        mPort->BSRRL = mPin;
    else
        mPort->BSRRH = mPin;
}
//---------------------------------------------------------------------------

void Gpio::writePort(unsigned short value)
{
    mPort->ODR = (mPort->ODR & (~mPin)) | (value & mPin);
}
 
unsigned short Gpio::readPort()
{
    if (mConfig.mode == modeOut)
        return mPort->ODR & mPin;
    else
        return mPort->IDR & mPin;
}
//---------------------------------------------------------------------------
