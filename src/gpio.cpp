#include "gpio.h"
#include "core/core.h"

#if defined(STM32F37X)
#define BSRRL   BSRR
#define BSRRH   BRR
#elif defined(STM32G4)
    #define IMR     IMR1
    #define EMR     EMR1
    #define RTSR    RTSR1
    #define FTSR    FTSR1
    #define PR      PR1
#endif

uint8_t Gpio::mPinsUsed[176]; // assuming all items = 0 at startup
NotifyEvent Gpio::m_interruptHandlers[16];

Gpio::Gpio(PinName pin, Flags flags/*, PinAF altFunction*/)
{
    mConfig.pin = pin;
    mConfig.flags = flags;
    mConfig.af = afNone;//altFunction;
    if (pin != noPin)
    {
        mPort = getPortByNumber(mConfig.portNumber);
        mPin = 1 << mConfig.pinNumber; 
    }
    else
    {
        mPort = 0L;
        mPin = 0;
    }
    config(mConfig.config);
}

Gpio::Gpio(Config conf)
{
    mConfig.config = conf;
    if (mConfig.pin != noPin)
    {
        mPort = getPortByNumber(mConfig.portNumber);
        mPin = 1 << mConfig.pinNumber; 
    }
    else
    {
        mPort = 0L;
        mPin = 0;
    }
    config(mConfig.config);
}

Gpio::Gpio(PortName port, uint16_t mask, Flags flags)
{
    mConfig.pin = port;
    mConfig.flags = flags;
    mConfig.manyPins = 1;
    mConfig.mask = mask;
    mPort = getPortByNumber(mConfig.portNumber);
    mPin = mask;
    config(mConfig.config);
}

Gpio::Gpio(GPIO_TypeDef *gpio, int pin)
{
    int port = getPortNumber(gpio);
    mPin = 1 << pin;
    mPort = gpio;
    if (port >= 0) // hardware GPIO
    {
        mConfig.portNumber = port;
        mConfig.pinNumber = pin;
        mConfig.flags = flagsDefault;
        mConfig.af = afNone;
        mConfig.periph = 0;
        config(mConfig.config);
    }
    else
    {
        mConfig.pin = noPin;
        mConfig.flags = flagsDefault;
        mConfig.af = afNone;
        mConfig.periph = 0;
    }
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

void Gpio::config(PinName pin, bool value)
{
    ConfigStruct c;
    c.pin = pin;
    c.flags = Output;
    c.af = afNone;
    config(c.config);
    GPIO_TypeDef *port = getPortByNumber(c.portNumber);
    if (value)
        port->BSRR = 1 << c.pinNumber;
    else
        port->BSRR = 0x10000 << c.pinNumber;
}

void Gpio::config(const Config &conf)
{
    const ConfigStruct &c = reinterpret_cast<const ConfigStruct&>(conf);
    if (c.pin == noPin)
        return;

    GPIO_TypeDef *port = getPortByNumber(c.portNumber);
    if (!port)
        return;

#if defined(STM32F4)
    RCC->AHB1ENR |= (1 << c.portNumber); // enable port clocks
#elif defined(STM32L4) || defined(STM32G4)
    RCC->AHB2ENR |= (1 << c.portNumber); // enable port clocks
#endif

    uint16_t mask;
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
    uint8_t pinId = static_cast<uint8_t>(cfg.pin);
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
#if defined(GPIOG)
        case 0x6: return GPIOG;
#endif
#if defined(GPIOH)
        case 0x7: return GPIOH;
#endif
#if defined(GPIOI)
        case 0x8: return GPIOI;
#endif
#if defined(GPIOJ)
        case 0x9: return GPIOJ;
#endif
#if defined(GPIOK)
        case 0xA: return GPIOK;
#endif        
        default: return 0L;
    }
}
        
int Gpio::getPortNumber(GPIO_TypeDef *gpio)
{
    switch (reinterpret_cast<uint32_t>(gpio))
    {
    case GPIOA_BASE: return 0x0;
    case GPIOB_BASE: return 0x1;
    case GPIOC_BASE: return 0x2;
    case GPIOD_BASE: return 0x3;
    case GPIOE_BASE: return 0x4;
    case GPIOF_BASE: return 0x5;
#if defined(GPIOG)    
    case GPIOG_BASE: return 0x6;
#endif
#if defined(GPIOH)    
    case GPIOH_BASE: return 0x7;
#endif
#if defined(GPIOI)
    case GPIOI_BASE: return 0x8;
#endif
#if defined(GPIOJ)    
    case GPIOJ_BASE: return 0x9;
#endif
#if defined(GPIOK)    
    case GPIOK_BASE: return 0xA;
#endif
    default: return -1;
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

void Gpio::configInterrupt(NotifyEvent event, InterruptMode mode)
{
    setAsInput();
    int line = mConfig.pinNumber;
    uint32_t mask = 1 << line;
    if (EXTI->IMR & mask)
        THROW(Exception::ResourceBusy);
    
    rcc().setPeriphEnabled(SYSCFG);
    SYSCFG->EXTICR[line >> 2] = mConfig.portNumber << ((line & 3) << 2);
    
    if (mode & 1)
        EXTI->RTSR |= mask;
    else
        EXTI->RTSR &= ~mask;
    
    if (mode & 2)
        EXTI->FTSR |= mask;
    else
        EXTI->FTSR &= ~mask;
    
    m_interruptHandlers[line] = event;
    EXTI->IMR |= mask;
    
    IRQn_Type irqn;
    switch (line)
    {
    case 0: irqn = EXTI0_IRQn; break;
    case 1: irqn = EXTI1_IRQn; break;
    case 2: irqn = EXTI2_IRQn; break;
    case 3: irqn = EXTI3_IRQn; break;
    case 4: irqn = EXTI4_IRQn; break;
    case 5: 
    case 6:
    case 7:
    case 8:
    case 9:
        irqn = EXTI9_5_IRQn;
        break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        irqn = EXTI15_10_IRQn;
        break;
    default:;
    }
    NVIC_EnableIRQ(irqn);
}
//---------------------------------------------------------------------------

bool Gpio::read() const
{
    if (!mPort)
        return false;
    if (mConfig.mode == modeOut)
        return mPort->ODR & mPin;
    else
        return mPort->IDR & mPin;
}

void Gpio::write(bool value)
{
    if (!mPort)
        return;
//    if (value)
//        mPort->BSRRL = mPin;
//    else
//        mPort->BSRRH = mPin;
//    if (value)
//        mPort->BSRR = mPin;
//    else
//        mPort->BSRR = mPin << 16;
    if (value)
        mPort->ODR |= mPin;
    else
        mPort->ODR &= ~mPin;
}
//---------------------------------------------------------------------------

void Gpio::writePort(uint16_t value)
{
    if (mPort)
        mPort->ODR = (mPort->ODR & (~mPin)) | (value & mPin);
}

uint16_t Gpio::readPort()
{
    if (mConfig.mode == modeOut)
        return mPort->ODR & mPin;
    else
        return mPort->IDR & mPin;
}
//---------------------------------------------------------------------------

extern "C"
{

void EXTI0_IRQHandler()
{
    EXTI->PR = 1 << 0;
    Gpio::m_interruptHandlers[0]();
}

void EXTI1_IRQHandler()
{
    EXTI->PR = 1 << 1;
    Gpio::m_interruptHandlers[1]();
}

void EXTI2_IRQHandler()
{
    EXTI->PR = 1 << 2;
    Gpio::m_interruptHandlers[2]();
}

void EXTI3_IRQHandler()
{
    EXTI->PR = 1 << 3;
    Gpio::m_interruptHandlers[3]();
}

void EXTI4_IRQHandler()
{
    EXTI->PR = 1 << 4;
    Gpio::m_interruptHandlers[4]();
}

void EXTI9_5_IRQHandler()
{
    for (int i=5; i<9; i++)
    {
        uint32_t mask = 1 << i;
        if (EXTI->PR & mask)
        {
            EXTI->PR = mask;
            Gpio::m_interruptHandlers[i]();
        }
    }
}

void EXTI15_10_IRQHandler()
{
    for (int i=10; i<15; i++)
    {
        uint32_t mask = 1 << i;
        if (EXTI->PR & mask)
        {
            EXTI->PR = mask;
            Gpio::m_interruptHandlers[i]();
        }
    }
}

} // extern "C"
     