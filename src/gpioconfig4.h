#pragma once
// this file is included by gpio.h, don't include it explicitly

enum Config
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
    USART1_RX_PA10 =    PINCONFIG(PA10, modeAF | outPushPull | pullUp   | speed50MHz, afUsart1, 0x10), //!<.
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
    I2C3_SDA_PC9  =     PINCONFIG(PC9,  modeAF | outOpenDrain /* | pullUp*/ | speed50MHz, afI2C3, 0x30), //!<.
    I2C3_SCL_PA8  =     PINCONFIG(PA8,  modeAF | outOpenDrain /* | pullUp*/ | speed50MHz, afI2C3, 0x30), //!<.
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
    SPI1_SCK_PA5 =      PINCONFIG(PA5,  modeAF | speed100MHz, afSpi1, 0x10), //!.
    SPI1_MISO_PA6 =     PINCONFIG(PA6,  modeAF | speed100MHz | pullUp , afSpi1, 0x10), //!.
    SPI1_MOSI_PA7 =     PINCONFIG(PA7,  modeAF | speed100MHz, afSpi1, 0x10), //!.
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
    
    FMC_A0          =   PINCONFIG(PF0,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A1          =   PINCONFIG(PF1,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A2          =   PINCONFIG(PF2,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A3          =   PINCONFIG(PF3,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A4          =   PINCONFIG(PF4,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A5          =   PINCONFIG(PF5,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A6          =   PINCONFIG(PF12, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A7          =   PINCONFIG(PF13, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A8          =   PINCONFIG(PF14, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A9          =   PINCONFIG(PF15, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A10         =   PINCONFIG(PG0,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A11         =   PINCONFIG(PG1,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A12         =   PINCONFIG(PG2,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A13         =   PINCONFIG(PG3,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A14         =   PINCONFIG(PG4,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A15         =   PINCONFIG(PG5,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A16         =   PINCONFIG(PD11, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A17         =   PINCONFIG(PD12, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A18         =   PINCONFIG(PD13, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A19         =   PINCONFIG(PE3,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A20         =   PINCONFIG(PE4,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A21         =   PINCONFIG(PE5,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A22         =   PINCONFIG(PE6,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A23         =   PINCONFIG(PE2,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_A24         =   PINCONFIG(PG13, modeAF | speed50MHz, afFmc, 0x10),
    FMC_A25         =   PINCONFIG(PG14, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D0          =   PINCONFIG(PD14, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D1          =   PINCONFIG(PD15, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D2          =   PINCONFIG(PD0,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D3          =   PINCONFIG(PD1,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D4          =   PINCONFIG(PE7,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D5          =   PINCONFIG(PE8,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D6          =   PINCONFIG(PE9,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D7          =   PINCONFIG(PE10, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D8          =   PINCONFIG(PE11, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D9          =   PINCONFIG(PE12, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D10         =   PINCONFIG(PE13, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D11         =   PINCONFIG(PE14, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D12         =   PINCONFIG(PE15, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D13         =   PINCONFIG(PD8,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D14         =   PINCONFIG(PD9,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D15         =   PINCONFIG(PD10, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D16         =   PINCONFIG(PH8,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D17         =   PINCONFIG(PH9,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D18         =   PINCONFIG(PH10, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D19         =   PINCONFIG(PH11, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D20         =   PINCONFIG(PH12, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D21         =   PINCONFIG(PH13, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D22         =   PINCONFIG(PH14, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D23         =   PINCONFIG(PH15, modeAF | speed50MHz, afFmc, 0x10),
    FMC_D24         =   PINCONFIG(PI0,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D25         =   PINCONFIG(PI1,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D26         =   PINCONFIG(PI2,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D27         =   PINCONFIG(PI3,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D28         =   PINCONFIG(PI6,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D29         =   PINCONFIG(PI7,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D30         =   PINCONFIG(PI9,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_D31         =   PINCONFIG(PI10, modeAF | speed50MHz, afFmc, 0x10),
    FMC_NE1         =   PINCONFIG(PD7,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NCE2        =   FMC_NE1,
    FMC_NE2         =   PINCONFIG(PG9,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NCE3        =   FMC_NE2,
    FMC_NE3         =   PINCONFIG(PG10, modeAF | speed50MHz, afFmc, 0x10),
    FMC_NCE4_1      =   FMC_NE3,
    FMC_NCE4_2      =   PINCONFIG(PG11, modeAF | speed50MHz, afFmc, 0x10),
    FMC_NE4         =   PINCONFIG(PG12, modeAF | speed50MHz, afFmc, 0x10),
    FMC_CLK         =   PINCONFIG(PD3,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NOE         =   PINCONFIG(PD4,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NWE         =   PINCONFIG(PD5,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NWAIT       =   PINCONFIG(PD6,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NL          =   PINCONFIG(PB7,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_CF_NIORD    =   PINCONFIG(PF6,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_CF_NREG     =   PINCONFIG(PF7,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_CF_NIOWR    =   PINCONFIG(PF8,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_CF_CD       =   PINCONFIG(PF9,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_CF_INTR     =   PINCONFIG(PF10, modeAF | speed50MHz, afFmc, 0x10),
    FMC_NAND16_INT2 =   PINCONFIG(PG6,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NAND16_INT3 =   PINCONFIG(PG7,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NBL0        =   PINCONFIG(PE0,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NBL1        =   PINCONFIG(PE1,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NBL2        =   PINCONFIG(PI4,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_NBL3        =   PINCONFIG(PI5,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDCLK       =   PINCONFIG(PG8,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNRAS      =   PINCONFIG(PF11, modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNCAS      =   PINCONFIG(PG15, modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNWE_PC0   =   PINCONFIG(PC0,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDCKE0_PC3  =   PINCONFIG(PC3,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNE0_PC2   =   PINCONFIG(PC2,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDCKE1_PB5  =   PINCONFIG(PB5,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNE1_PB6   =   PINCONFIG(PB6,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNWE_PH5   =   PINCONFIG(PH5,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDCKE0_PH2  =   PINCONFIG(PH2,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNE0_PH3   =   PINCONFIG(PH3,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDCKE1_PH7  =   PINCONFIG(PH7,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDNE1_PH6   =   PINCONFIG(PH6,  modeAF | speed50MHz, afFmc, 0x10),
    FMC_SDRAM_BA0   =   FMC_A14,
    FMC_SDRAM_BA1   =   FMC_A15,
    FMC_NAND16_CLE  =   FMC_A16,
    FMC_NAND16_ALE  =   FMC_A17,
};