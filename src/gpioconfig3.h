#pragma once
// this file is included by gpio.h, don't include it explicitly

enum Config
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
};