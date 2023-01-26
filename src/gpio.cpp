#include "gpio.h"

#if defined(STM32F37X)
#define BSRRL   BSRR
#define BSRRH   BRR
#endif

unsigned char Gpio::mPinsUsed[140]; // assuming all items = 0 at startup

Gpio::Gpio(PinName pin, Flags flags/*, PinAF altFunction*/)
{
    mConfig.pin = pin;
    mConfig.flags = flags;
    mConfig.af = afNone;//altFunction;
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

Gpio::~Gpio()
{
    config(NoConfig);
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
    
    RCC->AHB1ENR |= (1 << c.portNumber); // enable port clocks
     
    unsigned short mask;  
    if (c.manyPins)
        mask = c.mask;
    else // only one pin initialization
        mask = 1 << c.pinNumber;
    
    for (int pin=0; pin<16; pin++)
    {
        if (!(mask & (1<<pin)))
            continue;
        
        ConfigStruct cc = c;
        cc.pinNumber = pin;
        usePin(cc);
    
        if (c.outType)               // if output type is open drain
            port->ODR |= (1 << pin); // initialize in Hi-Z state
        
        int pin_x2 = pin * 2;
        port->MODER = port->MODER & ~(0x3 << pin_x2) | (c.mode << pin_x2);
        if (c.mode == modeOut || c.mode == modeAF)
        {
            port->OSPEEDR = port->OSPEEDR & ~(0x3 << pin_x2) | (c.speed << pin_x2);
            port->OTYPER = port->OTYPER & ~(0x1 << pin) | (c.outType << pin);
        }
        port->PUPDR = port->PUPDR & ~(0x3 << pin_x2) | (c.pull << pin_x2);
        
        if (c.afNumber != afNone)
        {
            __IO uint32_t &AFR = port->AFR[pin >> 3];
            uint8_t bit = (pin & 0x7) << 2;
            AFR = AFR & ~(0xF << bit) | (c.afNumber << bit);
        }
    }
}

void Gpio::updateConfig()
{
    if (mConfig.pin == noPin)
        return;
    
    for (int pin=0; pin<16; pin++)
    {
        if (!(mPin & (1<<pin)))
            continue;
        
        int pin_x2 = pin * 2;
        mPort->MODER = mPort->MODER & ~(0x3 << pin_x2) | (mConfig.mode << pin_x2);
        if (mConfig.mode == modeOut || mConfig.mode == modeAF)
        {
            mPort->OSPEEDR = mPort->OSPEEDR & ~(0x3 << pin_x2) | (mConfig.speed << pin_x2);
            mPort->OTYPER = mPort->OTYPER & ~(0x1 << pin) | (mConfig.outType << pin);
        }
        mPort->PUPDR = mPort->PUPDR & ~(0x3 << pin_x2) | (mConfig.pull << pin_x2);
        
        if (mConfig.afNumber != afNone)
        {
            __IO uint32_t &AFR = mPort->AFR[pin >> 3];
            uint8_t bit = (pin & 0x7) << 2;
            AFR = AFR & ~(0xF << bit) | (mConfig.afNumber << bit);
        }
    }
}

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
    if (cfg.config == NoConfig)
        mPinsUsed[pinId] = 0x00;
    else if (mPinsUsed[pinId])
        THROW(Exception::ResourceBusy); // trap
    else
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
#elif !defined(STM32F37X) && !defined(STM32F446xx)
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
//    if (value)
//        mPort->BSRRL = mPin;
//    else
//        mPort->BSRRH = mPin;
    if (value)
        mPort->BSRR = mPin;
    else
        mPort->BSRR = mPin << 16;
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
