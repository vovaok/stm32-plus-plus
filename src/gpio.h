#ifndef _GPIO_H
#define _GPIO_H

#include <stdarg.h>
#include "stm32.h"
#include "core/core.h"

/*! Pin configuration macro.
    Intended for internal use.
*/

#define PINCONFIG(pin, flags, af, periph) ((pin) | ((flags)<<8) | ((af)<<16) | ((periph)<<24))
#define GpioConfigGetPeriph(cfg) (cfg >> 24)
#define GpioConfigGetPeriphNumber(cfg) (((unsigned long)cfg) >> 28)
#define GpioConfigGetPeriphChannel(cfg) ((cfg >> 24) & 0xF)
#define GpioConfigGetAF(cfg) ((cfg >> 16) & 0xFF)
#define GpioConfigGetFlags(cfg) ((cfg >> 8) & 0xFF)
#define GpioConfigGetName(cfg) (cfg & 0xFF)

//#define GPIO_HAS_PORT(x)    defined(GPIO##x)

/*! GPIO pin class.
    Provides easy interface for pin configuration, including alternate functions.
    One instance of this class controls only \b one pin.
    To use the interface, create an instance with given name, flags and alternate function;
    all these parameters are enumeration types, for details see constructor description.
    Also you can create an instance with predefined configuration of Gpio::Config type. \n
    \b Note: configuration applies at instance construction, so you can destroy it if you don't need pin control later.\n\n
    \b Examples:\n
    The code below configures PD0 pin to be used as RX pin of CAN1 peripheral:
    ~~~~~~~~~~~~~~~~~~~~~~~~~~
    Gpio::config(CAN1_RX_PD0);
    ~~~~~~~~~~~~~~~~~~~~~~~~~~
    Configure PE7 pin as input with pull-up (e.g. for button):
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Gpio someButton(PE7, modeIn | pullUp);
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
class Gpio
{
public:
    /*! Enumeration of pin names.
        Intended for short notation of port - pin pair.
        PA0 is PORTA pin 0
        PA1 is PORTA pin 1
        ...
        ...up to PORTI pin 11
    */
    typedef enum
    {
        PA0  = 0x00, PA1  = 0x01, PA2  = 0x02, PA3  = 0x03, PA4  = 0x04, PA5  = 0x05, PA6  = 0x06, PA7  = 0x07,
        PA8  = 0x08, PA9  = 0x09, PA10 = 0x0A, PA11 = 0x0B, PA12 = 0x0C, PA13 = 0x0D, PA14 = 0x0E, PA15 = 0x0F,
        PB0  = 0x10, PB1  = 0x11, PB2  = 0x12, PB3  = 0x13, PB4  = 0x14, PB5  = 0x15, PB6  = 0x16, PB7  = 0x17,
        PB8  = 0x18, PB9  = 0x19, PB10 = 0x1A, PB11 = 0x1B, PB12 = 0x1C, PB13 = 0x1D, PB14 = 0x1E, PB15 = 0x1F,
        PC0  = 0x20, PC1  = 0x21, PC2  = 0x22, PC3  = 0x23, PC4  = 0x24, PC5  = 0x25, PC6  = 0x26, PC7  = 0x27,
        PC8  = 0x28, PC9  = 0x29, PC10 = 0x2A, PC11 = 0x2B, PC12 = 0x2C, PC13 = 0x2D, PC14 = 0x2E, PC15 = 0x2F,
        PD0  = 0x30, PD1  = 0x31, PD2  = 0x32, PD3  = 0x33, PD4  = 0x34, PD5  = 0x35, PD6  = 0x36, PD7  = 0x37,
        PD8  = 0x38, PD9  = 0x39, PD10 = 0x3A, PD11 = 0x3B, PD12 = 0x3C, PD13 = 0x3D, PD14 = 0x3E, PD15 = 0x3F,
        PE0  = 0x40, PE1  = 0x41, PE2  = 0x42, PE3  = 0x43, PE4  = 0x44, PE5  = 0x45, PE6  = 0x46, PE7  = 0x47,
        PE8  = 0x48, PE9  = 0x49, PE10 = 0x4A, PE11 = 0x4B, PE12 = 0x4C, PE13 = 0x4D, PE14 = 0x4E, PE15 = 0x4F,
        PF0  = 0x50, PF1  = 0x51, PF2  = 0x52, PF3  = 0x53, PF4  = 0x54, PF5  = 0x55, PF6  = 0x56, PF7  = 0x57,
        PF8  = 0x58, PF9  = 0x59, PF10 = 0x5A, PF11 = 0x5B, PF12 = 0x5C, PF13 = 0x5D, PF14 = 0x5E, PF15 = 0x5F,
        PG0  = 0x60, PG1  = 0x61, PG2  = 0x62, PG3  = 0x63, PG4  = 0x64, PG5  = 0x65, PG6  = 0x66, PG7  = 0x67,
        PG8  = 0x68, PG9  = 0x69, PG10 = 0x6A, PG11 = 0x6B, PG12 = 0x6C, PG13 = 0x6D, PG14 = 0x6E, PG15 = 0x6F,
        PH0  = 0x70, PH1  = 0x71, PH2  = 0x72, PH3  = 0x73, PH4  = 0x74, PH5  = 0x75, PH6  = 0x76, PH7  = 0x77,
        PH8  = 0x78, PH9  = 0x79, PH10 = 0x7A, PH11 = 0x7B, PH12 = 0x7C, PH13 = 0x7D, PH14 = 0x7E, PH15 = 0x7F,
        PI0  = 0x80, PI1  = 0x81, PI2  = 0x82, PI3  = 0x83, PI4  = 0x84, PI5  = 0x85, PI6  = 0x86, PI7  = 0x87,
        PI8  = 0x88, PI9  = 0x89, PI10 = 0x8A, PI11 = 0x8B, PI12 = 0x8C, PI13 = 0x8D, PI14 = 0x8E, PI15 = 0x8F,
        PJ0  = 0x90, PJ1  = 0x91, PJ2  = 0x92, PJ3  = 0x93, PJ4  = 0x94, PJ5  = 0x95, PJ6  = 0x96, PJ7  = 0x97,
        PJ8  = 0x98, PJ9  = 0x99, PJ10 = 0x9A, PJ11 = 0x9B, PJ12 = 0x9C, PJ13 = 0x9D, PJ14 = 0x9E, PJ15 = 0x9F,
        PK0  = 0xA0, PK1  = 0xA1, PK2  = 0xA2, PK3  = 0xA3, PK4  = 0xA4, PK5  = 0xA5, PK6  = 0xA6, PK7  = 0xA7,
        PK8  = 0xA8, PK9  = 0xA9, PK10 = 0xAA, PK11 = 0xAB, PK12 = 0xAC, PK13 = 0xAD, PK14 = 0xAE, PK15 = 0xAF,
        noPin = 0xFF  //!< no pin choosed
    } PinName;

    /*! Enumeration of port names.
        Intended for many pins handling.
    */
    typedef enum
    {
        portA = 0x00,
        portB = 0x10,
        portC = 0x20,
        portD = 0x30,
        portE = 0x40,
        portF = 0x50,
        portG = 0x60,
        portH = 0x70,
        portI = 0x80,
        noPort = 0xFF
    } PortName;

    /*! Gpio configuration flags.
        Use OR'ed flags to describe configuration. For example, `modeOut | outOpenDrain | speed50MHz`
        means that pin will be configured as output with open drain and choosed speed.
    */
    typedef enum
    {
        modeIn = 0<<0,      //!< mode selection: input
        modeOut = 1<<0,     //!< mode selection: output
        modeAF = 2<<0,      //!< mode selection: alternate function
        modeAnalog = 3<<0,  //!< mode selection: analog
        outPushPull = 0<<2, //!< output type selection: push-pull
        outOpenDrain = 1<<2,//!< output type selection: open-drain
        pullNone = 0<<3,    //!< no pull the pin
        pullUp = 1<<3,      //!< pull up the pin
        pullDown = 2<<3,    //!< pull down the pin
        speed2MHz = 0<<5,   //!< desired pin speed for output or alternate function is low (2MHz)
        speed25MHz = 1<<5,  //!< desired pin speed for output or alternate function is medium (25 MHz)
        speed50MHz = 2<<5,  //!< desired pin speed for output or alternate function is fast (50 MHz)
        speed100MHz = 3<<5, //!< desired pin speed for output or alternate function is high (100 MHz)
        flagsDefault = modeIn | outPushPull | pullNone | speed2MHz, //!< default flags combination

        Input = modeIn | pullNone | speed100MHz,
        InputPullDown = modeIn | pullDown | speed100MHz,
        InputPullUp = modeIn | pullUp | speed100MHz,
        Output = modeOut | outPushPull | speed100MHz,
        OutputOpenDrain = modeOut | outOpenDrain | speed100MHz
    } Flags;

    /*!
        Gpio alternate function numbers.
        AF numbers are aliased by peripheral names for convenience.
    */
    typedef enum
    {
        afNone = 0xFF, //!< no alternate function
        afRtc50Hz = 0,/*!<.*/ afMco = 0,/*!<.*/ afTamper = 0,/*!<.*/ afSjw = 0,/*!<.*/ afTrace = 0,/*!<.*/
        afTim1 = 1,/*!<.*/ afTim2 = 1,/*!<.*/
        afTim3 = 2,/*!<.*/ afTim4 = 2,/*!<.*/ afTim5 = 2,/*!<.*/
        afTim8 = 3,/*!<.*/ afTim9 = 3,/*!<.*/ afTim10 = 3,/*!<.*/ afTim11 = 3,/*!<.*/
        afI2C1 = 4,/*!<.*/ afI2C2 = 4,/*!<.*/ afI2C3 = 4,/*!<.*/
        afSpi1 = 5,/*!<.*/ afSpi2 = 5,/*!<.*/ afSpi4 = 5, afSpi5 = 5, afSpi6 = 5,
        afSpi3 = 6,/*!<.*/
        afUsart1 = 7,/*!<.*/ afUsart2 = 7,/*!<.*/ afUsart3 = 7,/*!<.*/ afI2S3ext = 7,/*!<.*/
        afUart4 = 8,/*!<.*/ afUart5 = 8,/*!<.*/ afUsart6 = 8,/*!<.*/
        afCan1 = 9,/*!<.*/ afCan2 = 9,/*!<.*/ afTim12 = 9,/*!<.*/ afTim13 = 9,/*!<.*/ afTim14 = 9,/*!<.*/
        afOtgFs = 10,/*!<.*/ afOtgHs = 10,/*!<.*/
        afEth = 11,/*!<.*/
        afFmc = 12,/*!<.*/ afOtgHsFs = 12,/*!<.*/ afSdio = 12,/*!<.*/
        afDcmi = 13,/*!<.*/
        afLcd = 14,
        afEventOut = 15,/*!<.*/

        af0 = 0,
        af1 = 1,
        af2 = 2,
        af3 = 3,
        af4 = 4,
        af5 = 5,
        af6 = 6,
        af7 = 7,
        af8 = 8,
        af9 = 9,
        af10 = 10,
        af11 = 11,
        af12 = 12,
        af13 = 13,
        af14 = 14,
        af15 = 15,
    } PinAF;

    /*! Predefined pin configurations.
        You can construct a pin with one of these configurations.
        @note This declaration is located in the corresponding gpiconfigX.h
    */
    // enum GpioConfig = {...};
    #if defined(STM32F4) || defined(STM32L4) || defined(STM32G4)
    #include "gpioconfig4.h"
    #elif defined(STM32F3)
    #include "gpioconfig3.h"
    #endif

public:
    /*! Constructor with explicit pin configuration.
        You should use it if there is not necessary configuration available.
        But other constructor is more convenient.
    */
    Gpio(PinName pin, Flags flags = flagsDefault/*, PinAF altFunction = afNone*/);

    /*! Constructor with predefined pin configuration.
        Convenient for fast configuration.
    */
    Gpio(Config config);

    /*! Construct Gpio object with many pins of port handling.
        You should use it if there is not necessary configuration available.
        But other constructor is more convenient.
    */
    Gpio(PortName port, uint16_t mask = 0xFFFF, Flags flags = flagsDefault);

    ~Gpio();

    /*! Initialize pin with explicit configuration.
        This function is useful when you not need to create an instance
        for manipulating the pin in the future.\n
        Try better using another implementation that accepts predefined config.
    */
    static void config(PinName pin, Flags flags = flagsDefault, PinAF altFunction = afNone);

    /*! Initialize pin with predefined configuration.
        This function is useful when you not need to create an instance
        for manipulating the pin in the future.\n
        \b Note: Use of this function is preferred.
        \param conf Pin configuration, see Config for details.
    */
    static void config(const Config &conf);

    /*! Initialize \c count pins with predefined configurations.
        Convenient when initializing several pins in one line of code.
        Using example: config(2, CAN_RX_PB8, CAN_TX_PB9);
        \param count The count of the pins to configure.
        \param conf1 Configuration of the first pin.
    */
    static void config(int count, Config conf1, ...);

    /*! Получение номера блока периферии.
        Номер периферии определяется по конфигурации, переданной в конструктор.
        \return значение номера блока периферии.
        \example
        Gpio thePin(Gpio::TIM2_CH1_PA0);
        int timerNumber = thePin.periphNumber(); // timerNumber will be 2
    */
    inline uint8_t periphNumber() const {return mConfig.periphNumber;}

    /*! Получение номера канала периферии.
        Канал периферии определяется по конфигурации, переданной в конструктор.
        \return значение номера канала периферии.
        Если данный блок периферии не имеет каналов, возвращаемое значение равно 0.
        \example
        Gpio thePin(Gpio::TIM2_CH1_PA0);
        int channelNumber = thePin.channelNumber(); // channelNumber will be 1
    */
    inline uint8_t channelNumber() const {return mConfig.channel;}

    /*! Изменение конфигурации как вход без подтяжки.
        По идее, можно вызывать в любое время.
    */
    void setAsInput();

    /*! Изменение конфигурации как вход с подтяжкой к питанию.
        По идее, можно вызывать в любое время.
    */
    void setAsInputPullUp();

    /*! Изменение конфигурации как вход с подтяжкой к земле.
        По идее, можно вызывать в любое время.
    */
    void setAsInputPullDown();

    /*! Изменение конфигурации как выход.
        По идее, можно вызывать в любое время.
    */
    void setAsOutput();

    /*! Изменение конфигурации как выход с открытым стоком.
        По идее, можно вызывать в любое время.
    */
    void setAsOutputOpenDrain();

    /*! Чтение состояния ноги.
        \return \c true, если на ноге 1, \c false, если 0. Что логично.
    */
    bool read() const;

    /*! Запись состояния ноги.
        \param value \c true, чтобы выставить 1, \c false - 0. И это нормально.
    */
    void write(bool value);

#if defined (STM32F37X)
    /*! Установка ноги в 1 */
    inline void set() {mPort->BSRR = mPin;}
    /*! Сброс ноги в 0 */
    inline void reset() {mPort->BRR = mPin;}
#else
    /*! Установка ноги в 1 */
    inline void set() {mPort->BSRR = mPin;}//{mPort->BSRRL = mPin;}
    /*! Сброс ноги в 0 */
    inline void reset() {mPort->BSRR = mPin << 16;}
#endif

    /*! Инверсия состояния ноги */
    inline void toggle() {mPort->ODR ^= mPin;}

    /*! запись порта*/
    void writePort(uint16_t value);

    /*! чтение порта*/
    uint16_t readPort();

    /*! что за нога? */
    PinName pin() const {return static_cast<Gpio::PinName>(mConfig.pin);}

    /*! Возвращает false, если при конфигурации нога задана как Gpio::noPin */
    bool isValid() const {return mPin;}

private:
    typedef union
    {
        Config config;
        struct
        {
            uint8_t pin;
            uint8_t flags;
            uint8_t af;
            uint8_t periph; // peripheral index
        };
        struct
        {
            uint8_t pinNumber: 4;
            uint8_t portNumber: 4;
            uint8_t mode: 2;
            uint8_t outType: 1;
            uint8_t pull: 2;
            uint8_t speed: 2;
            uint8_t manyPins: 1; // several pins of port used, 'mPin' variable contains mask
            uint8_t afNumber;
            uint8_t channel: 4;
            uint8_t periphNumber: 4;
        };
        struct
        {
            uint16_t _dummy;
            uint16_t mask;
        };
    } ConfigStruct;

    static uint8_t mPinsUsed[140];
    static void usePin(const ConfigStruct &cfg);

    ConfigStruct mConfig;
    GPIO_TypeDef *mPort;
    uint16_t mPin;

    void init();
    static GPIO_TypeDef *getPortByNumber(int port);
    void updateConfig();
};

#endif
