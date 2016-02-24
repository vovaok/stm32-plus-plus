#include "gpio.h"

unsigned char Gpio::mPinsUsed[140]; // assuming all items = 0 at startup

Gpio::Gpio(PinName pin, Flags flags, PinAF altFunction)
{
    mConfig.pin = pin;
    mConfig.flags = flags;
    mConfig.af = altFunction;
    mPin = 1 << mConfig.pinNumber;
    mPort = getPortByNumber(mConfig.portNumber);
    config(mConfig.config);
}

Gpio::Gpio(Config conf)
{
    mConfig.config = conf;
    mPin = 1 << mConfig.pinNumber;
    mPort = getPortByNumber(mConfig.portNumber);
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
    
    usePin(c);
    
    GPIO_TypeDef *port = getPortByNumber(c.portNumber);
    if (!port)
        return;
    
    RCC_AHB1PeriphClockCmd(1 << c.portNumber, ENABLE); 
    
    if (c.afNumber != afNone)
        GPIO_PinAFConfig(port, c.pinNumber, c.afNumber);
    
    GPIO_InitTypeDef GPIO_InitStructure;  
    GPIO_InitStructure.GPIO_Pin = 1 << c.pinNumber; 
    GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)c.mode;
    GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)c.speed;
    GPIO_InitStructure.GPIO_OType = (GPIOOType_TypeDef)c.outType;
    GPIO_InitStructure.GPIO_PuPd  = (GPIOPuPd_TypeDef)c.pull;
    GPIO_Init(port, &GPIO_InitStructure);
}

void Gpio::updateConfig()
{
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
        case 0x6: return GPIOG;
        case 0x7: return GPIOH;
        case 0x8: return GPIOI;
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
    if (mConfig.mode == modeOut)
        return GPIO_ReadOutputDataBit(mPort, mPin) == Bit_SET;
    else
        return GPIO_ReadInputDataBit(mPort, mPin) == Bit_SET;  
}

void Gpio::write(bool value)
{
    if (value)
        mPort->BSRRL = mPin;
    else
        mPort->BSRRH = mPin;
}