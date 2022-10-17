#ifndef _GPIO_H
#define _GPIO_H

#include <stdarg.h>
#include "stm32f4xx.h"
//#include "stm32_conf.h"
#include "core/core.h"

/*! Pin configuration macro.
    Intended for internal use.
*/

//#if defined (STM32F37X)
#define PINCONFIG(pin, flags, af, periph) ((pin) | ((flags)<<8) | ((af)<<16) | ((periph)<<24))
#define GpioConfigGetPeriph(cfg) (cfg >> 24)
#define GpioConfigGetPeriphNumber(cfg) (((unsigned long)cfg) >> 28)
#define GpioConfigGetPeriphChannel(cfg) ((cfg >> 24) & 0xF)
//#else
//#define PINCONFIG(pin, flags, af) ((pin) | ((flags)<<8) | ((af)<<16))
//#endif
#define GpioConfigGetAF(cfg) ((cfg >> 16) & 0xFF)
#define GpioConfigGetFlags(cfg) ((cfg >> 8) & 0xFF)
#define GpioConfigGetName(cfg) (cfg & 0xFF)

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
#if !defined(STM32F37X)
        PG0  = 0x60, PG1  = 0x61, PG2  = 0x62, PG3  = 0x63, PG4  = 0x64, PG5  = 0x65, PG6  = 0x66, PG7  = 0x67,
        PG8  = 0x68, PG9  = 0x69, PG10 = 0x6A, PG11 = 0x6B, PG12 = 0x6C, PG13 = 0x6D, PG14 = 0x6E, PG15 = 0x6F,
        PH0  = 0x70, PH1  = 0x71, PH2  = 0x72, PH3  = 0x73, PH4  = 0x74, PH5  = 0x75, PH6  = 0x76, PH7  = 0x77,
        PH8  = 0x78, PH9  = 0x79, PH10 = 0x7A, PH11 = 0x7B, PH12 = 0x7C, PH13 = 0x7D, PH14 = 0x7E, PH15 = 0x7F,
        PI0  = 0x80, PI1  = 0x81, PI2  = 0x82, PI3  = 0x83, PI4  = 0x84, PI5  = 0x85, PI6  = 0x86, PI7  = 0x87,
        PI8  = 0x88, PI9  = 0x89, PI10 = 0x8A, PI11 = 0x8B,
#endif
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
#if !defined(STM32F37X)        
        portG = 0x60,
        portH = 0x70,
        portI = 0x80,
#endif
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
#if !defined(STM32F37X)
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
        afFsmc = 12,/*!<.*/ afOtgHsFs = 12,/*!<.*/ afSdio = 12,/*!<.*/
        afDcmi = 13,/*!<.*/
        afEventOut = 15/*!<.*/
    } PinAF;
#else
    typedef enum
    {
        afNone = 0xFF, //!< no alternate function
        af0 = 0,
        af1 = 1,
        af2 = 2,
        af3 = 3,
        af4 = 4,
        af5 = 5,
        af6 = 6,
        af7 = 7, afUsart1 = 7, afUsart2 = 7, afUsart3 = 7,
        af8 = 8,
        af9 = 9,
        af10 = 10,
        af11 = 11,
        af12 = 12,
        af13 = 13,
        af14 = 14,
        af15 = 15,
//        afRtc50Hz = 0,/*!<.*/ afMco = 0,/*!<.*/ afTamper = 0,/*!<.*/ afSjw = 0,/*!<.*/ afTrace = 0,/*!<.*/
//        afTim2 = 1,/*!<.*/ afTim15 = 1,/*!<.*/  afTim16 = 1,/*!<.*/  afTim17 = 1,/*!<.*/ 
//        afTim3 = 2,/*!<.*/ afTim4 = 2,/*!<.*/ afTim5 = 2,/*!<.*/ afTim13 = 2,/*!<.*/ afTim14 = 2,/*!<.*/  afTim19 = 2,/*!<.*/ 
//        afTsc = 3,/*!<.*/ 
//        afI2C1 = 4,/*!<.*/ afI2C2 = 4,/*!<.*/ afI2C3 = 4,/*!<.*/
//        afSpi1 = 5,/*!<.*/ afSpi2 = 5,/*!<.*/
//        afSpi3 = 6,/*!<.*/
//        afUsart1 = 7,/*!<.*/ afUsart2 = 7,/*!<.*/ afUsart3 = 7,/*!<.*/ afI2S3ext = 7,/*!<.*/
//        afUart4 = 8,/*!<.*/ afUart5 = 8,/*!<.*/ afUsart6 = 8,/*!<.*/
//        afCan = 9,/*!<.*/  afTim12 = 9,/*!<.*/ afTim13 = 9,/*!<.*/ afTim14 = 9,/*!<.*/
//        afOtgFs = 10,/*!<.*/ afOtgHs = 10,/*!<.*/
//        afEth = 11,/*!<.*/
//        afFsmc = 12,/*!<.*/ afOtgHsFs = 12,/*!<.*/ afSdio = 12,/*!<.*/
//        afDcmi = 13,/*!<.*/
//        afEventOut = 15/*!<.*/
    } PinAF;
#endif
    
    /*! Predefined pin configurations.
        You can construct a pin with one of these configurations.
    */
#if !defined(STM32F37X)
    typedef enum
    {
        NoConfig =          PINCONFIG(noPin, flagsDefault, afNone, 0x00), //!< empty configuration
        // timers
        // TIM1
        TIM1_BKIN_PA6 =     PINCONFIG(PA6,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x10), //!<.
        TIM1_CH1N_PA7 =     PINCONFIG(PA7,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x19), //!<.
        TIM1_CH1_PA8 =      PINCONFIG(PA8,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x11), //!<.
        TIM1_CH2_PA9 =      PINCONFIG(PA9,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x12), //!<.
        TIM1_CH3_PA10 =     PINCONFIG(PA10, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x13), //!<.
        TIM1_CH4_PA11 =     PINCONFIG(PA11, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x14), //!<.
        TIM1_ETR_PA12 =     PINCONFIG(PA12, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x10), //!<.
        TIM1_CH2N_PB0 =     PINCONFIG(PB0,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x1A), //!<.
        TIM1_CH3N_PB1 =     PINCONFIG(PB1,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x1B), //!<.
        TIM1_BKIN_PB12 =    PINCONFIG(PB12, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x10), //!<.
        TIM1_CH1N_PB13 =    PINCONFIG(PB13, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x19), //!<.
        TIM1_CH2N_PB14 =    PINCONFIG(PB14, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x1A), //!<.
        TIM1_CH3N_PB15 =    PINCONFIG(PB15, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x1B), //!<.
        TIM1_ETR_PE7 =      PINCONFIG(PE7,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x10), //!<.
        TIM1_CH1N_PE8 =     PINCONFIG(PE8,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x19), //!<.
        TIM1_CH1_PE9 =      PINCONFIG(PE9,  modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x11), //!<.
        TIM1_CH2N_PE10 =    PINCONFIG(PE10, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x1A), //!<.
        TIM1_CH2_PE11 =     PINCONFIG(PE11, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x12), //!<.
        TIM1_CH3N_PE12 =    PINCONFIG(PE12, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x1B), //!<.
        TIM1_CH3_PE13 =     PINCONFIG(PE13, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x13), //!<.
        TIM1_CH4_PE14 =     PINCONFIG(PE14, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x14), //!<.
        TIM1_BKIN_PE15 =    PINCONFIG(PE15, modeAF | outPushPull | pullNone | speed50MHz, afTim1, 0x10), //!<.
        // TIM2
        TIM2_CH1_PA0 =      PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x21), //!<.
        TIM2_CH2_PA1 =      PINCONFIG(PA1,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x22), //!<.
        TIM2_CH3_PA2 =      PINCONFIG(PA2,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x23), //!<.
        TIM2_CH4_PA3 =      PINCONFIG(PA3,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x24), //!<.
        TIM2_CH1_PA5 =      PINCONFIG(PA5,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x21), //!<.
        TIM2_CH1_PA15 =     PINCONFIG(PA15, modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x21), //!<.
        TIM2_CH2_PB3 =      PINCONFIG(PB3,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x22), //!<.
        TIM2_CH3_PB10 =     PINCONFIG(PB10, modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x23), //!<.
        TIM2_CH4_PB11 =     PINCONFIG(PB11, modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x24), //!<.
        TIM2_ETR_PA0 =      PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x20), //!<.
        TIM2_ETR_PA5 =      PINCONFIG(PA5,  modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x20), //!<.
        TIM2_ETR_PA15 =     PINCONFIG(PA15, modeAF | outPushPull | pullNone | speed50MHz, afTim2, 0x20), //!<.
        // TIM3
        TIM3_CH1_PA6 =      PINCONFIG(PA6,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x31), //!<.
        TIM3_CH2_PA7 =      PINCONFIG(PA7,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x32), //!<.
        TIM3_CH3_PB0 =      PINCONFIG(PB0,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x33), //!<.
        TIM3_CH4_PB1 =      PINCONFIG(PB1,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x34), //!<.
        TIM3_CH1_PB4 =      PINCONFIG(PB4,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x31), //!<.
        TIM3_CH2_PB5 =      PINCONFIG(PB5,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x32), //!<.
        TIM3_CH1_PC6 =      PINCONFIG(PC6,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x31), //!<.
        TIM3_CH2_PC7 =      PINCONFIG(PC7,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x32), //!<.
        TIM3_CH3_PC8 =      PINCONFIG(PC8,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x33), //!<.
        TIM3_CH4_PC9 =      PINCONFIG(PC9,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x34), //!<.
        TIM3_ETR     =      PINCONFIG(PD2,  modeAF | outPushPull | pullNone | speed50MHz, afTim3, 0x30), //!<.
        // TIM4
        TIM4_CH1_PB6 =      PINCONFIG(PB6,  modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x41), //!<.
        TIM4_CH2_PB7 =      PINCONFIG(PB7,  modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x42), //!<.
        TIM4_CH3_PB8 =      PINCONFIG(PB8,  modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x43), //!<.
        TIM4_CH4_PB9 =      PINCONFIG(PB9,  modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x44), //!<.
        TIM4_CH1_PD12 =     PINCONFIG(PD12, modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x41), //!<.
        TIM4_CH2_PD13 =     PINCONFIG(PD13, modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x42), //!<.
        TIM4_CH3_PD14 =     PINCONFIG(PD14, modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x43), //!<.
        TIM4_CH4_PD15 =     PINCONFIG(PD15, modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x44), //!<.
        TIM4_ETR =          PINCONFIG(PE0,  modeAF | outPushPull | pullNone | speed50MHz, afTim4, 0x40), //!<.
        // TIM5
        TIM5_CH1_PA0 =      PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x51), //!<.
        TIM5_CH2_PA1 =      PINCONFIG(PA1,  modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x52), //!<.
        TIM5_CH3_PA2 =      PINCONFIG(PA2,  modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x53), //!<.
        TIM5_CH4_PA3 =      PINCONFIG(PA3,  modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x54), //!<.
        TIM5_CH1_PH10 =     PINCONFIG(PH10, modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x51), //!<.
        TIM5_CH2_PH11 =     PINCONFIG(PH11, modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x52), //!<.
        TIM5_CH3_PH12 =     PINCONFIG(PH12, modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x53), //!<.
        TIM5_CH4_PI0 =      PINCONFIG(PI0,  modeAF | outPushPull | pullNone | speed50MHz, afTim5, 0x54), //!<.
        // TIM8
        TIM8_ETR_PA0 =      PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x80), //!<.
        TIM8_CH1N_PA5 =     PINCONFIG(PA5,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x89), //!<.
        TIM8_BKIN_PA6 =     PINCONFIG(PA6,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x80), //!<.
        TIM8_CH1N_PA7 =     PINCONFIG(PA7,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x89), //!<.
        TIM8_CH2N_PB0 =     PINCONFIG(PB0,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x8A), //!<.
        TIM8_CH3N_PB1 =     PINCONFIG(PB1,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x8B), //!<.
        TIM8_CH2N_PB14 =    PINCONFIG(PB14, modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x8A), //!<.
        TIM8_CH3N_PB15 =    PINCONFIG(PB15, modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x8B), //!<.
        TIM8_CH1_PC6 =      PINCONFIG(PC6,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x81), //!<.
        TIM8_CH2_PC7 =      PINCONFIG(PC7,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x82), //!<.
        TIM8_CH3_PC8 =      PINCONFIG(PC8,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x83), //!<.
        TIM8_CH4_PC9 =      PINCONFIG(PC9,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x84), //!<.
        TIM8_CH1N_PH13 =    PINCONFIG(PH13, modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x89), //!<.
        TIM8_CH2N_PH14 =    PINCONFIG(PH14, modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x8A), //!<.
        TIM8_CH3N_PH15 =    PINCONFIG(PH15, modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x8B), //!<.
        TIM8_CH4_PI2 =      PINCONFIG(PI2,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x84), //!<.
        TIM8_ETR_PI3 =      PINCONFIG(PI3,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x80), //!<.
        TIM8_BKIN_PI4 =     PINCONFIG(PI4,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x80), //!<.
        TIM8_CH1_PI5 =      PINCONFIG(PI5,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x81), //!<.
        TIM8_CH2_PI6 =      PINCONFIG(PI6,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x82), //!<.
        TIM8_CH3_PI7 =      PINCONFIG(PI7,  modeAF | outPushPull | pullNone | speed50MHz, afTim8, 0x83), //!<.
        // TIM9
        TIM9_CH1_PA2 =      PINCONFIG(PA2,  modeAF | outPushPull | pullNone | speed50MHz, afTim9, 0x91), //!<.
        TIM9_CH2_PA3 =      PINCONFIG(PA3,  modeAF | outPushPull | pullNone | speed50MHz, afTim9, 0x92), //!<.
        TIM9_CH1_PE5 =      PINCONFIG(PE5,  modeAF | outPushPull | pullNone | speed50MHz, afTim9, 0x91), //!<.
        TIM9_CH2_PE6 =      PINCONFIG(PE6,  modeAF | outPushPull | pullNone | speed50MHz, afTim9, 0x92), //!<.
        // TIM10
        TIM10_CH1_PB8 =     PINCONFIG(PB8,  modeAF | outPushPull | pullNone | speed50MHz, afTim10, 0xA1), //!<.
        TIM10_CH1_PF6 =     PINCONFIG(PF6,  modeAF | outPushPull | pullNone | speed50MHz, afTim10, 0xA1), //!<.
        // TIM11
        TIM11_CH1_PB9 =     PINCONFIG(PB9,  modeAF | outPushPull | pullNone | speed50MHz, afTim11, 0xB1), //!<.
        TIM11_CH1_PF7 =     PINCONFIG(PF7,  modeAF | outPushPull | pullNone | speed50MHz, afTim11, 0xB1), //!<.
        // TIM12
        TIM12_CH1_PB14 =    PINCONFIG(PB14, modeAF | outPushPull | pullNone | speed50MHz, afTim12, 0xC1), //!<.
        TIM12_CH2_PB15 =    PINCONFIG(PB15, modeAF | outPushPull | pullNone | speed50MHz, afTim12, 0xC2), //!<.
        TIM12_CH1_PH6 =     PINCONFIG(PH6,  modeAF | outPushPull | pullNone | speed50MHz, afTim12, 0xC1), //!<.
        TIM12_CH2_PH9 =     PINCONFIG(PH9,  modeAF | outPushPull | pullNone | speed50MHz, afTim12, 0xC2), //!<.
        // TIM13
        TIM13_CH1_PA6 =     PINCONFIG(PA6,  modeAF | outPushPull | pullNone | speed50MHz, afTim13, 0xD1), //!<.
        TIM13_CH1_PF8 =     PINCONFIG(PF8,  modeAF | outPushPull | pullNone | speed50MHz, afTim13, 0xD1), //!<.
        // TIM14
        TIM14_CH1_PA7 =     PINCONFIG(PA7,  modeAF | outPushPull | pullNone | speed50MHz, afTim14, 0xE1), //!<.
        TIM14_CH1_PF9 =     PINCONFIG(PF9,  modeAF | outPushPull | pullNone | speed50MHz, afTim14, 0xE1), //!<.        
        // USART1
        USART1_CK_PA8 =     PINCONFIG(PA8,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_TX_PA9 =     PINCONFIG(PA9,  modeAF | outPushPull | pullUp   | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RX_PA10 =    PINCONFIG(PA10, modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_CTS_PA11 =   PINCONFIG(PA11, modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RTS_PA12 =   PINCONFIG(PA12, modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_TX_PB6 =     PINCONFIG(PB6,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RX_PB7  =    PINCONFIG(PB7,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        // USART2
        USART2_CTS_PA0 =    PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RTS_PA1 =    PINCONFIG(PA1,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_TX_PA2 =     PINCONFIG(PA2,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RX_PA3 =     PINCONFIG(PA3,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CK_PA4 =     PINCONFIG(PA4,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CTS_PD3 =    PINCONFIG(PD3,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RTS_PD4 =    PINCONFIG(PD4,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_TX_PD5 =     PINCONFIG(PD5,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RX_PD6 =     PINCONFIG(PD6,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CK_PD7 =     PINCONFIG(PD7,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        // USART 3
        USART3_TX_PB10 =    PINCONFIG(PB10, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RX_PB11 =    PINCONFIG(PB11, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CK_PB12 =    PINCONFIG(PB12, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CTS_PB13 =   PINCONFIG(PB13, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RTS_PB14 =   PINCONFIG(PB14, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_TX_PC10 =    PINCONFIG(PC10, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RX_PC11 =    PINCONFIG(PC11, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CK_PC12 =    PINCONFIG(PC12, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_TX_PD8 =     PINCONFIG(PD8,  modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RX_PD9 =     PINCONFIG(PD9,  modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CK_PD10 =    PINCONFIG(PD10, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CTS_PD11 =   PINCONFIG(PD11, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RTS_PD12 =   PINCONFIG(PD12, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        // UART4
        UART4_TX_PA0 =      PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, afUart4, 0x40), //!<.
        UART4_RX_PA1 =      PINCONFIG(PA1,  modeAF | outPushPull | pullNone | speed50MHz, afUart4, 0x40), //!<.
        UART4_TX_PC10 =     PINCONFIG(PC10, modeAF | outPushPull | pullNone | speed50MHz, afUart4, 0x40), //!<.
        UART4_RX_PC11 =     PINCONFIG(PC11, modeAF | outPushPull | pullNone | speed50MHz, afUart4, 0x40), //!<.
        // UART5
        UART5_TX_PC12 =     PINCONFIG(PC12, modeAF | outPushPull | pullNone | speed50MHz, afUart5, 0x50), //!<.
        UART5_RX_PD2 =      PINCONFIG(PD2,  modeAF | outPushPull | pullNone | speed50MHz, afUart5, 0x50), //!<.
        // USART6
        USART6_TX_PC6 =     PINCONFIG(PC6,  modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_RX_PC7 =     PINCONFIG(PC7,  modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_CK_PC8 =     PINCONFIG(PC8,  modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_CK_PG7 =     PINCONFIG(PG7,  modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_RTS_PG8 =    PINCONFIG(PG8,  modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_RX_PG9 =     PINCONFIG(PG9,  modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_RTS_PG12 =   PINCONFIG(PG12, modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_CTS_PG13 =   PINCONFIG(PG13, modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_TX_PG14 =    PINCONFIG(PG14, modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        USART6_CTS_PG15 =   PINCONFIG(PG15, modeAF | outPushPull | pullNone | speed50MHz, afUsart6, 0x60), //!<.
        // CAN1
        CAN1_RX_PA11 =      PINCONFIG(PA11, modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        CAN1_TX_PA12 =      PINCONFIG(PA12, modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        CAN1_RX_PB8 =       PINCONFIG(PB8,  modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        CAN1_TX_PB9 =       PINCONFIG(PB9,  modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        CAN1_RX_PD0 =       PINCONFIG(PD0,  modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        CAN1_TX_PD1 =       PINCONFIG(PD1,  modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        CAN1_RX_PI9 =       PINCONFIG(PI9,  modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        CAN1_TX_PH13 =      PINCONFIG(PH13, modeAF | outPushPull | pullUp | speed50MHz, afCan1, 0x10), //!<.
        // CAN2
        CAN2_RX_PB5 =       PINCONFIG(PB5,  modeAF | outPushPull | pullUp | speed50MHz, afCan2, 0x20), //!<.
        CAN2_TX_PB6 =       PINCONFIG(PB6,  modeAF | outPushPull | pullUp | speed50MHz, afCan2, 0x20), //!<.
        CAN2_RX_PB12 =      PINCONFIG(PB12, modeAF | outPushPull | pullUp | speed50MHz, afCan2, 0x20), //!<.
        CAN2_TX_PB13 =      PINCONFIG(PB13, modeAF | outPushPull | pullUp | speed50MHz, afCan2, 0x20), //!<.
        
        //I2C
        I2C2_SCL_PH4 =      PINCONFIG(PH4,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C2, 0x20), //!<.
        I2C2_SDA_PH5 =      PINCONFIG(PH5,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C2, 0x20), //!<.
        I2C2_SCL_PB10 =     PINCONFIG(PB10,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C2, 0x20), //!<.
        I2C2_SDA_PB11 =     PINCONFIG(PB11,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C2, 0x20), //!<.
        I2C2_SMBA_PH6 =     PINCONFIG(PH6,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C2, 0x20), //!<.
        I2C3_SCL_PH7 =      PINCONFIG(PH7,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C3, 0x30), //!<.
        I2C3_SDA_PH8  =     PINCONFIG(PH8,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C3, 0x30), //!<.
        I2C3_SMBA_PH9 =     PINCONFIG(PH9,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C3, 0x30), //!<.
        I2C2_SMBA_PB12  =   PINCONFIG(PB12,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C2, 0x20), //!<.
        I2C3_SDA_PC9  =     PINCONFIG(PC9,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C3, 0x30), //!<.
        I2C3_SCL_PA8  =     PINCONFIG(PA8,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C3, 0x30), //!<.
        I2C3_SMBA_PA9  =    PINCONFIG(PA9,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C3, 0x30), //!<.
        I2C1_SMBA_PB5  =    PINCONFIG(PB5,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C1, 0x10), //!<.
        I2C1_SCL_PB6  =     PINCONFIG(PB6,  modeAF | outOpenDrain| pullNone | speed50MHz, afI2C1, 0x10), //!<.
        I2C1_SDA_PB7  =     PINCONFIG(PB7,  modeAF | outOpenDrain| pullNone | speed50MHz, afI2C1, 0x10), //!<.
        I2C1_SCL_PB8  =     PINCONFIG(PB8,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C1, 0x10), //!<.
        I2C1_SDA_PB9  =     PINCONFIG(PB9,  modeAF | outOpenDrain| pullUp | speed50MHz, afI2C1, 0x10), //!<.
          
        // DAC
        DAC_OUT1_PA4  =     PINCONFIG(PA4,  modeAnalog, afNone, 0x11), //!<.
        DAC_OUT2_PA5  =     PINCONFIG(PA5,  modeAnalog, afNone, 0x12), //!<.
          
        // ADC
        ADC1_IN0_PA0 =      PINCONFIG(PA0, modeAnalog, afNone, 0x10), //!<.
        ADC1_IN1_PA1 =      PINCONFIG(PA1, modeAnalog, afNone, 0x11), //!<.
        ADC1_IN2_PA2 =      PINCONFIG(PA2, modeAnalog, afNone, 0x12), //!<.
        ADC1_IN3_PA3 =      PINCONFIG(PA3, modeAnalog, afNone, 0x13), //!<.
        ADC1_IN4_PA4 =      PINCONFIG(PA4, modeAnalog, afNone, 0x14), //!<.
        ADC1_IN5_PA5 =      PINCONFIG(PA5, modeAnalog, afNone, 0x15), //!<.
        ADC1_IN6_PA6 =      PINCONFIG(PA6, modeAnalog, afNone, 0x16), //!<.
        ADC1_IN7_PA7 =      PINCONFIG(PA7, modeAnalog, afNone, 0x17), //!<.
        ADC1_IN8_PB0 =      PINCONFIG(PB0, modeAnalog, afNone, 0x18), //!<.
        ADC1_IN9_PB1 =      PINCONFIG(PB1, modeAnalog, afNone, 0x19), //!<.
        ADC1_IN10_PC0 =     PINCONFIG(PC0, modeAnalog, afNone, 0x1A), //!<.
        ADC1_IN11_PC1 =     PINCONFIG(PC1, modeAnalog, afNone, 0x1B), //!<.
        ADC1_IN12_PC2 =     PINCONFIG(PC2, modeAnalog, afNone, 0x1C), //!<.
        ADC1_IN13_PC3 =     PINCONFIG(PC3, modeAnalog, afNone, 0x1D), //!<.
        ADC1_IN14_PC4 =     PINCONFIG(PC4, modeAnalog, afNone, 0x1E), //!<.
        ADC1_IN15_PC5 =     PINCONFIG(PC5, modeAnalog, afNone, 0x1F), //!<.
          
        ADC2_IN0_PA0 =      PINCONFIG(PA0, modeAnalog, afNone, 0x20), //!<.
        ADC2_IN1_PA1 =      PINCONFIG(PA1, modeAnalog, afNone, 0x21), //!<.
        ADC2_IN2_PA2 =      PINCONFIG(PA2, modeAnalog, afNone, 0x22), //!<.
        ADC2_IN3_PA3 =      PINCONFIG(PA3, modeAnalog, afNone, 0x23), //!<.
        ADC2_IN4_PA4 =      PINCONFIG(PA4, modeAnalog, afNone, 0x24), //!<.
        ADC2_IN5_PA5 =      PINCONFIG(PA5, modeAnalog, afNone, 0x25), //!<.
        ADC2_IN6_PA6 =      PINCONFIG(PA6, modeAnalog, afNone, 0x26), //!<.
        ADC2_IN7_PA7 =      PINCONFIG(PA7, modeAnalog, afNone, 0x27), //!<.
        ADC2_IN8_PB0 =      PINCONFIG(PB0, modeAnalog, afNone, 0x28), //!<.
        ADC2_IN9_PB1 =      PINCONFIG(PB1, modeAnalog, afNone, 0x29), //!<.  
        ADC2_IN10_PC0 =     PINCONFIG(PC0, modeAnalog, afNone, 0x2A), //!<.
        ADC2_IN11_PC1 =     PINCONFIG(PC1, modeAnalog, afNone, 0x2B), //!<.
        ADC2_IN12_PC2 =     PINCONFIG(PC2, modeAnalog, afNone, 0x2C), //!<.
        ADC2_IN13_PC3 =     PINCONFIG(PC3, modeAnalog, afNone, 0x2D), //!<.
        ADC2_IN14_PC4 =     PINCONFIG(PC4, modeAnalog, afNone, 0x2E), //!<.
        ADC2_IN15_PC5 =     PINCONFIG(PC5, modeAnalog, afNone, 0x2F), //!<.
        
        ADC3_IN0_PA0 =      PINCONFIG(PA0, modeAnalog, afNone, 0x30), //!<.
        ADC3_IN1_PA1 =      PINCONFIG(PA1, modeAnalog, afNone, 0x31), //!<.
        ADC3_IN2_PA2 =      PINCONFIG(PA2, modeAnalog, afNone, 0x32), //!<.
        ADC3_IN3_PA3 =      PINCONFIG(PA3, modeAnalog, afNone, 0x33), //!<.
        ADC3_IN10_PC0 =     PINCONFIG(PC0, modeAnalog, afNone, 0x3A), //!<.
        ADC3_IN11_PC1 =     PINCONFIG(PC1, modeAnalog, afNone, 0x3B), //!<.
        ADC3_IN12_PC2 =     PINCONFIG(PC2, modeAnalog, afNone, 0x3C), //!<.
        ADC3_IN13_PC3 =     PINCONFIG(PC3, modeAnalog, afNone, 0x3D), //!<.
        
        // SPI (I2S)
        SPI1_NSS_PA4 =      PINCONFIG(PA4,  modeAF | speed50MHz, afSpi1, 0x10), //!.
        SPI1_SCK_PA5 =      PINCONFIG(PA5,  modeAF | speed50MHz, afSpi1, 0x10), //!.
        SPI1_MISO_PA6 =     PINCONFIG(PA6,  modeAF | speed50MHz | pullUp , afSpi1, 0x10), //!.
        SPI1_MOSI_PA7 =     PINCONFIG(PA7,  modeAF | speed50MHz, afSpi1, 0x10), //!.
        SPI1_NSS_PA15 =     PINCONFIG(PA15, modeAF | speed50MHz, afSpi1, 0x10), //!.
        SPI1_SCK_PB3 =      PINCONFIG(PB3,  modeAF | speed50MHz, afSpi1, 0x10), //!.
        SPI1_MISO_PB4 =     PINCONFIG(PB4,  modeAF | speed50MHz | pullUp, afSpi1, 0x10), //!.
        SPI1_MOSI_PB5 =     PINCONFIG(PB5,  modeAF | speed50MHz, afSpi1, 0x10), //!.
        SPI2_NSS_PB9 =      PINCONFIG(PB9,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_WS_PB9 =       PINCONFIG(PB9,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_SCK_PB10 =     PINCONFIG(PB10, modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_CK_PB10 =      PINCONFIG(PB10, modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_NSS_PB12 =     PINCONFIG(PB12, modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_WS_PB12 =      PINCONFIG(PB12, modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_SCK_PB13 =     PINCONFIG(PB13, modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_CK_PB13 =      PINCONFIG(PB13, modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_MISO_PB14 =    PINCONFIG(PB14, modeAF | speed50MHz | pullUp, afSpi2, 0x20), //!.
        SPI2_MOSI_PB15 =    PINCONFIG(PB15, modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_SD_PB15 =      PINCONFIG(PB15, modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_MISO_PC2 =     PINCONFIG(PC2,  modeAF | speed50MHz | pullUp, afSpi2, 0x20), //!.
        SPI2_MOSI_PC3 =     PINCONFIG(PC3,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_SD_PC3 =       PINCONFIG(PC3,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_MCK_PC6 =      PINCONFIG(PC6,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S_CKIN_PC9 =      PINCONFIG(PC9,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_NSS_PI0 =      PINCONFIG(PI0,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_WS_PI0 =       PINCONFIG(PI0,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_SCK_PI1 =      PINCONFIG(PI1,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_CK_PI1 =       PINCONFIG(PI1,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        SPI2_MISO_PI2 =     PINCONFIG(PI2,  modeAF | speed50MHz | pullUp, afSpi2, 0x20), //!.
        SPI2_MOSI_PI3 =     PINCONFIG(PI3,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        I2S2_SD_PI3 =       PINCONFIG(PI3,  modeAF | speed50MHz, afSpi2, 0x20), //!.
        // nedodelano I2S2ext!!!!!
        SPI3_NSS_PA4 =      PINCONFIG(PA4,  modeAF | speed50MHz, afSpi3, 0x30), //!.
        I2S3_WS_PA4 =       PINCONFIG(PA4,  modeAF | speed50MHz, afSpi3, 0x30), //!.
        SPI3_NSS_PA15 =     PINCONFIG(PA15, modeAF | speed50MHz, afSpi3, 0x30), //!.
        I2S3_WS_PA15 =      PINCONFIG(PA15, modeAF | speed50MHz, afSpi3, 0x30), //!.
        SPI3_SCK_PB3 =      PINCONFIG(PB3,  modeAF | speed50MHz, afSpi3, 0x30), //!.
        I2S3_CK_PB3 =       PINCONFIG(PB3,  modeAF | speed50MHz, afSpi3, 0x30), //!.
        SPI3_MISO_PB4 =     PINCONFIG(PB4,  modeAF | speed50MHz | pullUp, afSpi3, 0x30), //!.
        SPI3_MOSI_PB5 =     PINCONFIG(PB5,  modeAF | speed50MHz, afSpi3, 0x30), //!.
        I2S3_SD_PB5 =       PINCONFIG(PB5,  modeAF | speed50MHz, afSpi3, 0x30), //!.
        I2S3_MCK_PC7 =      PINCONFIG(PC7,  modeAF | speed50MHz, afSpi3, 0x30), //!.
        SPI3_SCK_PC10 =     PINCONFIG(PC10, modeAF | speed50MHz, afSpi3, 0x30), //!.
        I2S3_CK_PC10 =      PINCONFIG(PC10, modeAF | speed50MHz, afSpi3, 0x30), //!.
        SPI3_MISO_PC11 =    PINCONFIG(PC11, modeAF | speed50MHz | pullUp, afSpi3, 0x30), //!.
        SPI3_MOSI_PC12 =    PINCONFIG(PC12, modeAF | speed50MHz, afSpi3, 0x30), //!.
        I2S3_SD_PC12 =      PINCONFIG(PC12, modeAF | speed50MHz, afSpi3, 0x30), //!.
        // nedodelano I2S3ext
        SPI4_NSS_PE11 =     PINCONFIG(PE11, modeAF | speed50MHz, afSpi4, 0x40),
        SPI4_SCK_PE12 =     PINCONFIG(PE12, modeAF | speed50MHz, afSpi4, 0x40),
        SPI4_MISO_PE13 =    PINCONFIG(PE13, modeAF | speed50MHz | pullUp, afSpi4, 0x40),
        SPI4_MOSI_PE14 =    PINCONFIG(PE14, modeAF | speed50MHz, afSpi4, 0x40),
        SPI5_NSS_PF6 =      PINCONFIG(PF6,  modeAF | speed50MHz, afSpi5, 0x50),
        SPI5_SCK_PF7 =      PINCONFIG(PF7,  modeAF | speed50MHz, afSpi5, 0x50),
        SPI5_MISO_PF8 =     PINCONFIG(PF8,  modeAF | speed50MHz | pullUp, afSpi5, 0x50),
        SPI5_MOSI_PF9 =     PINCONFIG(PF9,  modeAF | speed50MHz, afSpi5, 0x50),
        SPI6_NSS_PG8 =      PINCONFIG(PG8,  modeAF | speed50MHz, afSpi6, 0x60),
        SPI6_SCK_PG13 =     PINCONFIG(PG13, modeAF | speed50MHz, afSpi6, 0x60),
        SPI6_MISO_PG12 =    PINCONFIG(PG12, modeAF | speed50MHz | pullUp, afSpi6, 0x60),
        SPI6_MOSI_PG14 =    PINCONFIG(PG14, modeAF | speed50MHz, afSpi6, 0x60),
        // OTG_FS
        OTG_FS_SOF =        PINCONFIG(PA8,  modeAF | speed100MHz, afOtgFs, 0x10), //!<.
      //  OTG_FS_VBUS =       PINCONFIG(PA9,  modeAF | speed100MHz, afOtgFs, 0x10), //!<.
      //  OTG_FS_ID =         PINCONFIG(PA10, modeAF | outOpenDrain | pullUp | speed100MHz, afOtgFs, 0x10), //!<.
        OTG_FS_DM =         PINCONFIG(PA11, modeAF | speed100MHz, afOtgFs, 0x10), //!<.
        OTG_FS_DP =         PINCONFIG(PA12, modeAF | speed100MHz, afOtgFs, 0x10), //!<.
        // OTG_HS
        OTG_HS_SOF =        PINCONFIG(PA4,  modeAF | speed100MHz, afOtgHsFs, 0x20), //!<.
        OTG_HS_ID =         PINCONFIG(PB12, modeAF | speed100MHz, afOtgHsFs, 0x20), //!<.
        OTG_HS_VBUS =       PINCONFIG(PB13, modeIn | speed100MHz, afNone   , 0x20), //!<.
        OTG_HS_DM =         PINCONFIG(PB14, modeAF | speed100MHz, afOtgHsFs, 0x20), //!<.
        OTG_HS_DP =         PINCONFIG(PB15, modeAF | speed100MHz, afOtgHsFs, 0x20), //!<.
        // OTG_HS (ULPI)
        OTG_HS_ULPI_D0 =    PINCONFIG(PA3,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_D1 =    PINCONFIG(PB0,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_D2 =    PINCONFIG(PB1,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_D3 =    PINCONFIG(PB10, modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_D4 =    PINCONFIG(PB11, modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_D5 =    PINCONFIG(PB12, modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_D6 =    PINCONFIG(PB13, modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_D7 =    PINCONFIG(PB5,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_CLK =   PINCONFIG(PA5,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_NXT =   PINCONFIG(PC3,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_DIR =   PINCONFIG(PC2,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_STP =   PINCONFIG(PC0,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_NXT_PH4=PINCONFIG(PH4,  modeAF | speed100MHz, afOtgHs, 0x21), //!<.
        OTG_HS_ULPI_DIR_PI11=PINCONFIG(PI11,modeAF | speed100MHz, afOtgHs, 0x21), //!<.\n to be continued....

        MCO1            =   PINCONFIG(PA8,  modeAF | speed100MHz, afMco, 0x10),
        MCO2            =   PINCONFIG(PC9,  modeAF | speed100MHz, afMco, 0x20),
    } Config;
#else
    
    typedef enum
    {
        NoConfig =          PINCONFIG(noPin, flagsDefault, afNone, 0x00), //!< empty configuration
        // timers
        // TIM2
        TIM2_CH1_PA0 =      PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, af1, 0x21), //!<.
        TIM2_CH2_PA1 =      PINCONFIG(PA1,  modeAF | outPushPull | pullNone | speed50MHz, af1, 0x22), //!<.
        TIM2_CH3_PA2 =      PINCONFIG(PA2,  modeAF | outPushPull | pullNone | speed50MHz, af1, 0x23), //!<.
        TIM2_CH4_PA3 =      PINCONFIG(PA3,  modeAF | outPushPull | pullNone | speed50MHz, af1, 0x24), //!<.
        // TIM12
        TIM12_CH1_PB14 =    PINCONFIG(PB14, modeAF | outPushPull | pullNone | speed50MHz, af9, 0xc1),
        TIM12_CH2_PB15 =    PINCONFIG(PB15, modeAF | outPushPull | pullNone | speed50MHz, af9, 0xc2),
        // TIM15
        TIM15_CH1_PB14 =    PINCONFIG(PB14, modeAF | outPushPull | pullNone | speed50MHz, af1, 0xf1),
        TIM15_CH1N_PB15 =   PINCONFIG(PB15, modeAF | outPushPull | pullNone | speed50MHz, af2, 0xf9),
        TIM15_CH2_PB15 =    PINCONFIG(PB15, modeAF | outPushPull | pullNone | speed50MHz, af1, 0xf2),
        
        // USART1
        USART1_CK_PA8 =     PINCONFIG(PA8,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_TX_PA9 =     PINCONFIG(PA9,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RX_PA10 =    PINCONFIG(PA10, modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_CTS_PA11 =   PINCONFIG(PA11, modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RTS_PA12 =   PINCONFIG(PA12, modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_TX_PB6 =     PINCONFIG(PB6,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RX_PB7  =    PINCONFIG(PB7,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_TX_PC4 =     PINCONFIG(PC4,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RX_PC5  =    PINCONFIG(PC5,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_TX_PE0 =     PINCONFIG(PE0,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        USART1_RX_PE1  =    PINCONFIG(PE1,  modeAF | outPushPull | pullNone | speed50MHz, afUsart1, 0x10), //!<.
        // USART2
        USART2_CTS_PA0 =    PINCONFIG(PA0,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RTS_PA1 =    PINCONFIG(PA1,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_TX_PA2 =     PINCONFIG(PA2,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RX_PA3 =     PINCONFIG(PA3,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CK_PA4 =     PINCONFIG(PA4,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_TX_PB3 =     PINCONFIG(PB3,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RX_PB4 =     PINCONFIG(PB4,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CK_PB5 =     PINCONFIG(PB5,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CTS_PD3 =    PINCONFIG(PD3,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RTS_PD4 =    PINCONFIG(PD4,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_TX_PD5 =     PINCONFIG(PD5,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_RX_PD6 =     PINCONFIG(PD6,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CK_PD7 =     PINCONFIG(PD7,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        USART2_CK_PF7 =     PINCONFIG(PF7,  modeAF | outPushPull | pullNone | speed50MHz, afUsart2, 0x20), //!<.
        // USART 3
        USART3_CTS_PA13 =   PINCONFIG(PA13, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_TX_PB8 =     PINCONFIG(PB8,  modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RX_PB9 =     PINCONFIG(PB9,  modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_TX_PB10 =    PINCONFIG(PB10, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RTS_PB14 =   PINCONFIG(PB14, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_TX_PC10 =    PINCONFIG(PC10, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RX_PC11 =    PINCONFIG(PC11, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CK_PC12 =    PINCONFIG(PC12, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_TX_PD8 =     PINCONFIG(PD8,  modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RX_PD9 =     PINCONFIG(PD9,  modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CK_PD10 =    PINCONFIG(PD10, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_CTS_PD11 =   PINCONFIG(PD11, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RTS_PD12 =   PINCONFIG(PD12, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RX_PE15 =    PINCONFIG(PE15, modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        USART3_RTS_PF6 =    PINCONFIG(PF6,  modeAF | outPushPull | pullNone | speed50MHz, afUsart3, 0x30), //!<.
        
        // DAC1
        DAC1_OUT1_PA4 =     PINCONFIG(PA4,  modeAnalog, afNone, 0x11),
        DAC1_OUT2_PA5 =     PINCONFIG(PA5,  modeAnalog, afNone, 0x12),
        DAC2_OUT1_PA6 =     PINCONFIG(PA6,  modeAnalog, afNone, 0x21),
        
        // SDADC1
        SDADC1_AIN0P =      PINCONFIG(PE12, modeAnalog, afNone, 0x10),
        SDADC1_AIN0M =      PINCONFIG(PE13, modeAnalog, afNone, 0x10),
        SDADC1_AIN1P =      PINCONFIG(PE11, modeAnalog, afNone, 0x11),
        SDADC1_AIN2P =      PINCONFIG(PE10, modeAnalog, afNone, 0x12),
        SDADC1_AIN2M =      PINCONFIG(PE11, modeAnalog, afNone, 0x12),
        SDADC1_AIN3P =      PINCONFIG(PE7,  modeAnalog, afNone, 0x13),
        SDADC1_AIN4P =      PINCONFIG(PB2,  modeAnalog, afNone, 0x14),
        SDADC1_AIN4M =      PINCONFIG(PE7,  modeAnalog, afNone, 0x14),
        SDADC1_AIN5P =      PINCONFIG(PB1,  modeAnalog, afNone, 0x15),
        SDADC1_AIN6P =      PINCONFIG(PB0,  modeAnalog, afNone, 0x16),
        SDADC1_AIN6M =      PINCONFIG(PB1,  modeAnalog, afNone, 0x16),   
        SDADC1_AIN7P =      PINCONFIG(PE9,  modeAnalog, afNone, 0x17),
        SDADC1_AIN8P =      PINCONFIG(PE8,  modeAnalog, afNone, 0x18),
        SDADC1_AIN8M =      PINCONFIG(PE9,  modeAnalog, afNone, 0x18),
        // SDADC2
        SDADC2_AIN0P =      PINCONFIG(PE15, modeAnalog, afNone, 0x20),
        SDADC2_AIN0M =      PINCONFIG(PB10, modeAnalog, afNone, 0x20),
        SDADC2_AIN1P =      PINCONFIG(PE14, modeAnalog, afNone, 0x21),
        SDADC2_AIN2P =      PINCONFIG(PE13, modeAnalog, afNone, 0x22),
        SDADC2_AIN2M =      PINCONFIG(PE14, modeAnalog, afNone, 0x22),
        SDADC2_AIN3P =      PINCONFIG(PE12, modeAnalog, afNone, 0x23),
        SDADC2_AIN4P =      PINCONFIG(PE11, modeAnalog, afNone, 0x24),
        SDADC2_AIN4M =      PINCONFIG(PE12, modeAnalog, afNone, 0x24),
        SDADC2_AIN5P =      PINCONFIG(PE7,  modeAnalog, afNone, 0x25),
        SDADC2_AIN6P =      PINCONFIG(PB2,  modeAnalog, afNone, 0x26),  
        SDADC2_AIN6M =      PINCONFIG(PE7,  modeAnalog, afNone, 0x26),
        SDADC2_AIN7P =      PINCONFIG(PE9,  modeAnalog, afNone, 0x27),
        SDADC2_AIN8P =      PINCONFIG(PE8,  modeAnalog, afNone, 0x28),
        SDADC2_AIN8M =      PINCONFIG(PE9,  modeAnalog, afNone, 0x28), 
        // SDADC3
        SDADC3_AIN0P =      PINCONFIG(PD14, modeAnalog, afNone, 0x30),
        SDADC3_AIN0M =      PINCONFIG(PD15, modeAnalog, afNone, 0x30),
        SDADC3_AIN1P =      PINCONFIG(PD13, modeAnalog, afNone, 0x31),
        SDADC3_AIN2P =      PINCONFIG(PD12, modeAnalog, afNone, 0x32),
        SDADC3_AIN2M =      PINCONFIG(PD13, modeAnalog, afNone, 0x32),
        SDADC3_AIN3P =      PINCONFIG(PD11, modeAnalog, afNone, 0x33),
        SDADC3_AIN4P =      PINCONFIG(PD10, modeAnalog, afNone, 0x34),
        SDADC3_AIN4M =      PINCONFIG(PD11, modeAnalog, afNone, 0x34),
        SDADC3_AIN5P =      PINCONFIG(PD9,  modeAnalog, afNone, 0x35),
        SDADC3_AIN6P =      PINCONFIG(PD8,  modeAnalog, afNone, 0x36),
        SDADC3_AIN6M =      PINCONFIG(PD9,  modeAnalog, afNone, 0x36),
        SDADC3_AIN7P =      PINCONFIG(PB15, modeAnalog, afNone, 0x37),    
        SDADC3_AIN8P =      PINCONFIG(PB14, modeAnalog, afNone, 0x38),
        SDADC3_AIN8M =      PINCONFIG(PB15, modeAnalog, afNone, 0x38),
} Config;
    
#endif
    
private:
    typedef union
    {
        Config config;
        struct
        {
            unsigned char pin;
            unsigned char flags;
            unsigned char af;
            unsigned char periph; // peripheral index
        };
        struct
        {
            unsigned char pinNumber: 4;
            unsigned char portNumber: 4;
            unsigned char mode: 2;
            unsigned char outType: 1;
            unsigned char pull: 2;
            unsigned char speed: 2;
            unsigned char manyPins: 1; // several pins of port used, 'mPin' variable contains mask
            unsigned char afNumber;
            unsigned char channel: 4;
            unsigned char periphNumber: 4;
        };
        struct
        {
            unsigned short _dummy;
            unsigned short mask;
        };
    } ConfigStruct;
  
    static unsigned char mPinsUsed[140];
    static void usePin(const ConfigStruct &cfg);
    ConfigStruct mConfig;
    GPIO_TypeDef *mPort;
    unsigned short mPin;
    
    void init();
    static GPIO_TypeDef *getPortByNumber(int port);
    void updateConfig();
    
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
    Gpio(PortName port, unsigned short mask = 0xFFFF, Flags flags = flagsDefault);
    
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
    inline unsigned char periphNumber() const {return mConfig.periphNumber;}
    
    /*! Получение номера канала периферии.
        Канал периферии определяется по конфигурации, переданной в конструктор.
        \return значение номера канала периферии.
        Если данный блок периферии не имеет каналов, возвращаемое значение равно 0.
        \example
        Gpio thePin(Gpio::TIM2_CH1_PA0);
        int channelNumber = thePin.channelNumber(); // channelNumber will be 1
    */
    inline unsigned char channelNumber() const {return mConfig.channel;}
    
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
    void writePort(unsigned short value);

    /*! чтение порта*/
    unsigned short readPort();
    
    /*! что за нога? */
    PinName pin() const {return static_cast<Gpio::PinName>(mConfig.pin);}
    
    /*! Возвращает false, если при конфигурации нога задана как Gpio::noPin */
    bool isValid() const {return mPin;}
};

#endif
