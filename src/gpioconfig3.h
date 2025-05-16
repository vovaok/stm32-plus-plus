#pragma once
// this file is included by gpio.h, don't include it explicitly

enum Config
{
    NoConfig =          PINCONFIG(noPin, flagsDefault, afNone, 0x00), //!< empty configuration
  
    
    // DAC1
    DAC1_OUT1_PA4 =     PINCONFIG(PA4,  modeAnalog, afNone, 0x11),
    DAC1_OUT2_PA5 =     PINCONFIG(PA5,  modeAnalog, afNone, 0x12),
    DAC2_OUT1_PA6 =     PINCONFIG(PA6,  modeAnalog, afNone, 0x21),
    
    
      // ADC1
    DECL_ADC_PIN(1, 1, PA0),
    DECL_ADC_PIN(1, 2, PA1),
    DECL_ADC_PIN(1, 3, PA2),
    DECL_ADC_PIN(1, 4, PA3),   
    DECL_ADC_PIN(1, 6, PC0),
    DECL_ADC_PIN(1, 7, PC1),
    DECL_ADC_PIN(1, 8, PC2),
    DECL_ADC_PIN(1, 9, PC3),   
    DECL_ADC_PIN(1, 11, PB0),
    DECL_ADC_PIN(1, 12, PB1),
    DECL_ADC_PIN(1, 13, PB13),
    
      // ADC2
    DECL_ADC_PIN(2, 1, PA4),
    DECL_ADC_PIN(2, 2, PA5),
    DECL_ADC_PIN(2, 3, PA6),
    DECL_ADC_PIN(2, 4, PA7),
    DECL_ADC_PIN(2, 5, PC4),
    DECL_ADC_PIN(2, 6, PC0),
    DECL_ADC_PIN(2, 7, PC1),
    DECL_ADC_PIN(2, 8, PC2),
    DECL_ADC_PIN(2, 9, PC3),   
    DECL_ADC_PIN(2, 11, PC5),
    DECL_ADC_PIN(2, 12, PB2),
    DECL_ADC_PIN(2, 13, PB12),
    DECL_ADC_PIN(2, 14, PB14),
    DECL_ADC_PIN(2, 15, PB15),
  
  
    
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
    
    
#if defined(STM32F303xC)
    // Alternate function 0
#define _CURRENT_AF     0
    MCO_PA8 =           PINAF(PA8, 0, 0),
    RTC_REFIN_PA1 =     PINAF(PA1, 0, 0),
    SWDIO_ITMS_PA13 =   PINAF(PA13, 0, 0),
    SWCLK_JTCK_PA14 =   PINAF(PA14, 0, 0),
    JTDI_PA15 =         PINAF(PA15, 0, 0),
    JTDO_TRACESWO_PB3 = PINAF(PB3, 0, 0),
    NJTRST_PB4 =        PINAF(PB4, 0, 0),
#undef _CURRENT_AF

// Alternate function 1
#define _CURRENT_AF     1
    // Port A
    TIM2_CH1_ETR_PA0 =  PINAF(PA0, 2, 1),  // TIM2 is device 2, channel 1
    TIM2_CH2_PA1 =      PINAF(PA1, 2, 2),
    TIM2_CH3_PA2 =      PINAF(PA2, 2, 3),
    TIM2_CH4_PA3 =      PINAF(PA3, 2, 4),
    TIM3_CH2_PA4 =      PINAF(PA4, 3, 2),
    TIM2_CH1_ETR_PA5 =  PINAF(PA5, 2, 1),
    TIM16_CH1_PA6 =     PINAF(PA6, 16, 1),
    TIM17_CH1_PA7 =     PINAF(PA7, 17, 1),
    TIM17_BKIN_PA10 =   PINAF(PA10, 17, 0),  // BKIN doesn't have channel
    TIM16_CH1_PA12 =    PINAF(PA12, 16, 1),
    TIM16_CH1N_PA13 =   PINAF(PA13, 16, 1|8),
    TIM2_CH1_ETR_PA15 = PINAF(PA15, 2, 1),
    
    // Port B
    TIM2_CH2_PB3 =      PINAF(PB3, 2, 2),
    TIM16_CH1_PB4 =     PINAF(PB4, 16, 1),
    TIM16_BKIN_PB5 =    PINAF(PB5, 16, 0),
    TIM16_CH1_PB6 =     PINAF(PB6, 16, 1),
    TIM17_CH1_PB7 =     PINAF(PB7, 17, 1),
    TIM16_CH1_PB8 =     PINAF(PB8, 16, 1),
    TIM17_CH1_PB9 =     PINAF(PB9, 17, 1),
    TIM2_CH3_PB10 =     PINAF(PB10, 2, 3),
    TIM2_CH4_PB11 =     PINAF(PB11, 2, 4),
    TIM15_CH1_PB14 =    PINAF(PB14, 15, 1),
    RTC_REFIN_PB15 =    PINAF(PB15, 0, 0),
#undef _CURRENT_AF

// Alternate function 2
#define _CURRENT_AF     2
    // Port A
    TIM3_CH1_PA6 =      PINAF(PA6, 3, 1),
    TIM3_CH2_PA7 =      PINAF(PA7, 3, 2),
    TSC_G4_IO3_PA13 =   PINAF(PA13, 0, 0),  // TSC doesn't have device number
    TSC_G4_IO4_PA14 =   PINAF(PA14, 0, 0),
    TIM8_CH1_PA15 =     PINAF(PA15, 8, 1),
    
    // Port B
    TIM3_CH3_PB0 =      PINAF(PB0, 3, 3),
    TIM3_CH4_PB1 =      PINAF(PB1, 3, 4),
    TIM4_ETR_PB3 =      PINAF(PB3, 4, 0),  // ETR doesn't have channel
    TIM3_CH1_PB4 =      PINAF(PB4, 3, 1),
    TIM3_CH2_PB5 =      PINAF(PB5, 3, 2),
    TIM4_CH1_PB6 =      PINAF(PB6, 4, 1),
    TIM4_CH2_PB7 =      PINAF(PB7, 4, 2),
    TIM4_CH3_PB8 =      PINAF(PB8, 4, 3),
    TIM4_CH4_PB9 =      PINAF(PB9, 4, 4),
    TIM17_BKIN_PB4 =    PINAF(PB4, 17, 0),
    TIM17_CH1_PB5 =     PINAF(PB5, 17, 1),
    TIM3_ETR_PB12 =     PINAF(PB12, 3, 0),
#undef _CURRENT_AF

// Alternate function 3
#define _CURRENT_AF     3
    // Port A
    TSC_G1_IO1_PA0 =    PINAF(PA0, 0, 0),
    TSC_G1_IO2_PA1 =    PINAF(PA1, 0, 0),
    TSC_G1_IO3_PA2 =    PINAF(PA2, 0, 0),
    TSC_G1_IO4_PA3 =    PINAF(PA3, 0, 0),
    TSC_G2_IO1_PA4 =    PINAF(PA4, 0, 0),
    TSC_G2_IO2_PA5 =    PINAF(PA5, 0, 0),
    TSC_G2_IO3_PA6 =    PINAF(PA6, 0, 0),
    TSC_G2_IO4_PA7 =    PINAF(PA7, 0, 0),
    TSC_G4_IO1_PA9 =    PINAF(PA9, 0, 0),
    TSC_G4_IO2_PA10 =   PINAF(PA10, 0, 0),
    I2C1_SDA_PA14 =     PINAF(PA14, 1, 0),  // I2C1 is device 1
    I2C1_SCL_PA15 =     PINAF(PA15, 1, 0),
#undef _CURRENT_AF

// Alternate function 4
#define _CURRENT_AF     4
    // Port A
    SPI1_NSS_PA4 =      PINAF(PA4, 1, 0),  // SPI1 is device 1
    I2C2_SMBA_PA8 =     PINAF(PA8, 2, 0),   // I2C2 is device 2
    I2C2_SCL_PA9 =      PINAF(PA9, 2, 0),
    I2C2_SDA_PA10 =     PINAF(PA10, 2, 0),
    IR_OUT_PA13 =       PINAF(PA13, 0, 0),
    TIM8_CH2_PA14 =     PINAF(PA14, 8, 2),
    SPI1_NSS_PA15 =     PINAF(PA15, 1, 0),
    
    // Port B
    TIM8_CH2N_PB0 =     PINAF(PB0, 8, 2|8),
    TIM8_CH3N_PB1 =     PINAF(PB1, 8, 3|8),
    TIM8_CH1N_PB3 =     PINAF(PB3, 8, 1|8),
    TIM8_CH2N_PB4 =     PINAF(PB4, 8, 2|8),
    I2C1_SMBA_PB5 =     PINAF(PB5, 1, 0),
    I2C1_SCL_PB6 =      PINAF(PB6, 1, 0),
    I2C1_SDA_PB7 =      PINAF(PB7, 1, 0),
    I2C1_SCL_PB8 =      PINAF(PB8, 1, 0),
    I2C1_SDA_PB9 =      PINAF(PB9, 1, 0),
    I2C2_SMBA_PB12 =    PINAF(PB12, 2, 0),
#undef _CURRENT_AF

// Alternate function 5
#define _CURRENT_AF     5
    // Port A
    SPI1_SCK_PA5 =      PINAF(PA5, 1, 0),
    SPI1_MISO_PA6 =     PINAF(PA6, 1, 0),
    SPI1_MOSI_PA7 =     PINAF(PA7, 1, 0),
    I2S2_MCK_PA8 =      PINAF(PA8, 2, 0),  // I2S2 is device 2
    I2S3_MCK_PA9 =      PINAF(PA9, 3, 0),  // I2S3 is device 3
    SPI3_NSS_PA15 =     PINAF(PA15, 3, 0),
    
    // Port B
    SPI1_SCK_PB3 =      PINAF(PB3, 1, 0),
    SPI1_MISO_PB4 =     PINAF(PB4, 1, 0),
    SPI1_MOSI_PB5 =     PINAF(PB5, 1, 0),
    SPI2_NSS_PB12 =     PINAF(PB12, 2, 0),
    SPI2_SCK_PB13 =     PINAF(PB13, 2, 0),
    SPI2_MISO_PB14 =    PINAF(PB14, 2, 0),
    SPI2_MOSI_PB15 =    PINAF(PB15, 2, 0),
    
    // Port C
    I2S_CKIN_PC9 =      PINAF(PC9, 0, 0),
    UART4_TX_PC10 =     PINAF(PC10, 4, 0),
    UART4_RX_PC11 =     PINAF(PC11, 4, 0),
    UART5_TX_PC12 =     PINAF(PC12, 5, 0),
#undef _CURRENT_AF

// Alternate function 6
#define _CURRENT_AF     6
    // Port A
    TIM1_BKIN_PA6 =     PINAF(PA6, 1, 0),
    TIM1_CH1N_PA7 =     PINAF(PA7, 1, 1|8),
    TIM1_CH1_PA8 =      PINAF(PA8, 1, 1),
    TIM1_CH2_PA9 =      PINAF(PA9, 1, 2),
    TIM1_CH3_PA10 =     PINAF(PA10, 1, 3),
    TIM1_CH1N_PA11 =    PINAF(PA11, 1, 1|8),
    TIM1_CH2N_PA12 =    PINAF(PA12, 1, 2|8),
    TIM1_BKIN_PA14 =    PINAF(PA14, 1, 0),
    USART2_TX_PA14 =    PINAF(PA14, 2, 0),
    USART2_RX_PA15 =    PINAF(PA15, 2, 0),
    
    // Port B
    TIM1_CH2N_PB0 =     PINAF(PB0, 1, 2|8),
    TIM1_CH3N_PB1 =     PINAF(PB1, 1, 3|8),
    SPI3_SCK_PB3 =      PINAF(PB3, 3, 0),
    SPI3_MISO_PB4 =     PINAF(PB4, 3, 0),
    SPI3_MOSI_PB5 =     PINAF(PB5, 3, 0),
    IR_OUT_PB9 =        PINAF(PB9, 0, 0),
    TIM1_BKIN_PB12 =    PINAF(PB12, 1, 0),
    TIM1_CH1N_PB13 =    PINAF(PB13, 1, 1|8),
    TIM1_CH2N_PB14 =    PINAF(PB14, 1, 2|8),
    TIM1_CH3N_PB15 =    PINAF(PB15, 1, 3|8),
#undef _CURRENT_AF

// Alternate function 7
#define _CURRENT_AF     7
    // Port A
    USART2_CTS_PA0 =    PINAF(PA0, 2, 0),
    USART2_RTS_DE_PA1 = PINAF(PA1, 2, 0),
    USART2_TX_PA2 =     PINAF(PA2, 2, 0),
    USART2_RX_PA3 =     PINAF(PA3, 2, 0),
    USART2_CK_PA4 =     PINAF(PA4, 2, 0),
    USART1_CK_PA8 =     PINAF(PA8, 1, 0),
    USART1_TX_PA9 =     PINAF(PA9, 1, 0),
    USART1_RX_PA10 =    PINAF(PA10, 1, 0),
    USART1_CTS_PA11 =   PINAF(PA11, 1, 0),
    USART1_RTS_DE_PA12 = PINAF(PA12, 1, 0),
    USART3_CTS_PA13 =   PINAF(PA13, 3, 0),
    
    // Port B
    USART2_TX_PB3 =     PINAF(PB3, 2, 0),
    USART2_RX_PB4 =     PINAF(PB4, 2, 0),
    USART2_CK_PB5 =     PINAF(PB5, 2, 0),
    USART1_TX_PB6 =     PINAF(PB6, 1, 0),
    USART1_RX_PB7 =     PINAF(PB7, 1, 0),
    USART3_TX_PB10 =    PINAF(PB10, 3, 0),
    USART3_RX_PB11 =    PINAF(PB11, 3, 0),
    USART3_CK_PB12 =    PINAF(PB12, 3, 0),
    USART3_CTS_PB13 =   PINAF(PB13, 3, 0),
    USART3_RTS_DE_PB14 = PINAF(PB14, 3, 0),
#undef _CURRENT_AF

// Alternate function 8
#define _CURRENT_AF     8
    // Port A
    COMP1_OUT_PA0 =     PINAF(PA0, 1, 0),  // COMP1 is device 1
    TIM15_CH1N_PA1 =    PINAF(PA1, 15, 1|8),
    COMP2_OUT_PA2 =     PINAF(PA2, 2, 0),
    TIM15_CH1_PA2 =     PINAF(PA2, 15, 1),
    TIM15_CH2_PA3 =     PINAF(PA3, 15, 2),
    TIM8_BKIN_PA6 =     PINAF(PA6, 8, 0),
    TIM8_CH1N_PA7 =     PINAF(PA7, 8, 1|8),
    COMP3_OUT_PA8 =     PINAF(PA8, 3, 0),
    COMP5_OUT_PA9 =     PINAF(PA9, 5, 0),
    COMP6_OUT_PA10 =    PINAF(PA10, 6, 0),
    COMP1_OUT_PA11 =    PINAF(PA11, 1, 0),
    COMP2_OUT_PA12 =    PINAF(PA12, 2, 0),
    TIM1_BKIN_PA15 =    PINAF(PA15, 1, 0),
    
    // Port B
    COMP4_OUT_PB1 =     PINAF(PB1, 4, 0),
    COMP1_OUT_PB8 =     PINAF(PB8, 1, 0),
    COMP2_OUT_PB9 =     PINAF(PB9, 2, 0),
#undef _CURRENT_AF

// Alternate function 9
#define _CURRENT_AF     9
    // Port A
    TIM8_BKIN_PA0 =     PINAF(PA0, 8, 0),
    TIM15_BKIN_PA9 =    PINAF(PA9, 15, 0),
    TIM8_BKIN_PA10 =    PINAF(PA10, 8, 0),
    CAN_RX_PA11 =       PINAF(PA11, 1, 0),  // CAN1 is device 1
    CAN_TX_PA12 =       PINAF(PA12, 1, 0),
    
    // Port B
    CAN_RX_PB8 =        PINAF(PB8, 1, 0),
    CAN_TX_PB9 =        PINAF(PB9, 1, 0),
#undef _CURRENT_AF

// Alternate function 10
#define _CURRENT_AF     10
    // Port A
    TIM8_ETR_PA0 =      PINAF(PA0, 8, 0),
    TIM4_ETR_PA8 =      PINAF(PA8, 4, 0),
    TIM2_CH3_PA9 =      PINAF(PA9, 2, 3),
    TIM2_CH4_PA10 =     PINAF(PA10, 2, 4),
    TIM4_CH1_PA11 =     PINAF(PA11, 4, 1),
    TIM4_CH2_PA12 =     PINAF(PA12, 4, 2),
    TIM4_CH3_PA13 =     PINAF(PA13, 4, 3),
    
    // Port B
    TIM8_CH2_PB8 =      PINAF(PB8, 8, 2),
    TIM8_CH3_PB9 =      PINAF(PB9, 8, 3),
    TIM8_BKIN2_PB7 =    PINAF(PB7, 8, 0),
#undef _CURRENT_AF

// Alternate function 11
#define _CURRENT_AF     11
    // Port A
    TIM1_CH4_PA11 =     PINAF(PA11, 1, 4),
    TIM1_ETR_PA12 =     PINAF(PA12, 1, 0),
#undef _CURRENT_AF

// Alternate function 12
#define _CURRENT_AF     12
    // Port A
    TIM1_BKIN2_PA11 =   PINAF(PA11, 1, 0),
#undef _CURRENT_AF
#else
    
    
    #define _CURRENT_AF     1
    TIM2_CH1_PA0 =      PINAF(PA0, 2, 1),
    TIM2_CH2_PA1 =      PINAF(PA1, 2, 2),
    TIM2_CH3_PA2 =      PINAF(PA2, 2, 3),
    TIM2_CH4_PA3 =      PINAF(PA3, 2, 4),
    TIM2_CH1_PA5 =      PINAF(PA5, 2, 1),
    TIM16_CH1_PA6 =     PINAF(PA6, 16, 1),
    TIM17_CH1_PA7 =     PINAF(PA7, 17, 1),
    TIM17_BKIN_PA10 =   PINAF(PA10, 17, 0),
    TIM16_CH1_PA12 =    PINAF(PA12, 16, 1),
    TIM16_CH1N_PA13 =   PINAF(PA13, 16, 1|8),    
    TIM2_CH1_PA15 =     PINAF(PA15, 2, 1),   
    TIM2_CH2_PB3 =      PINAF(PB3, 2, 2),
    TIM16_CH1_PB4 =     PINAF(PB4, 16, 1),
    TIM16_BKIN_PB5 =    PINAF(PB5, 16, 0),
    TIM16_CH1N_PB6 =    PINAF(PB6, 16, 1|8),
    TIM17_CH1N_PB7 =    PINAF(PB7, 17, 1|8),
    TIM16_CH1_PB8 =     PINAF(PB8, 16, 1),
    TIM17_CH1_PB9 =     PINAF(PB9, 17, 1),
    TIM2_CH3_PB10 =     PINAF(PB10, 2, 3),
    TIM2_CH4_PB11 =     PINAF(PB11, 2, 4),
    TIM15_CH1_PB14 =    PINAF(PB14, 15, 1),
    TIM15_CH2_PB15 =    PINAF(PB15, 15, 2),   
    #undef _CURRENT_AF    
    
       // Alternate function 2
    #define _CURRENT_AF     2    
   
    TIM3_CH2_PA4 =      PINAF(PA4, 3, 2),   
    TIM3_CH1_PA6 =      PINAF(PA6, 3, 1),
    TIM3_CH2_PA7 =      PINAF(PA7, 3, 2),   
    TIM3_CH3_PB0 =      PINAF(PB0, 3, 3),
    TIM3_CH4_PB1 =      PINAF(PB1, 3, 4),  
    TIM3_CH1_PB4 =      PINAF(PB4, 3, 1),
    TIM3_CH2_PB5 =      PINAF(PB5, 3, 2),    
    TIM15_CH1N_PB15 =   PINAF(PB15, 15, 1|8),
    TIM1_CH1_PC0 =      PINAF(PC0, 1, 1),    
    TIM1_CH3_PC2 =      PINAF(PC2, 1, 3),
    TIM1_CH4_PC3 =      PINAF(PC3, 1, 4),
    TIM1_ETR_PC4 =      PINAF(PC4, 1, 0),
    TIM15_BKIN_PC5 =    PINAF(PC5, 15, 0),
    TIM3_CH1_PC6 =      PINAF(PC6, 3, 1),
    TIM3_CH2_PC7 =      PINAF(PC7, 3, 2),
    TIM3_CH3_PC8 =      PINAF(PC8, 3, 3),
    TIM3_CH4_PC9 =      PINAF(PC9, 3, 4),    
    TIM3_ETR_PD2 =      PINAF(PD2, 3, 0),    
#undef _CURRENT_AF    
 
     // Alternate function 3
#define _CURRENT_AF     3    
  
#undef _CURRENT_AF    
    
  
        // Alternate function 4
#define _CURRENT_AF     4       
    I2C1_SDA_PA14 =     PINAF(PA14, 1, 0),
    I2C1_SCL_PA15 =     PINAF(PA15, 1, 0),    
    I2C1_SMBA_PB5 =     PINAF(PB5, 1, 0),
    I2C1_SDA_PB7 =      PINAF(PB7, 1, 0),
    I2C1_SCL_PB8 =      PINAF(PB8, 1, 0),
    I2C1_SDA_PB9 =      PINAF(PB9, 1, 0),  
    TIM1_CH3N_PB15 =    PINAF(PB15, 1, 3|8),   
    TIM1_CH1N_PC13 =    PINAF(PC13, 1, 1|8), 
#undef _CURRENT_AF    
    
    
        // Alternate function 5
#define _CURRENT_AF     5    
    SPI1_NSS_PA4 =      PINAF(PA4, 1, 0),
    SPI1_SCK_PA5 =      PINAF(PA5, 1, 0),
    SPI1_MISO_PA6 =     PINAF(PA6, 1, 0),
    SPI1_MOSI_PA7 =     PINAF(PA7, 1, 0),   
    IR_OUT_PA13 =       PINAF(PA13, 1, 0),   
    SPI1_NSS_PA15 =     PINAF(PA15, 1, 0),
    SPI1_SCK_PB3 =      PINAF(PB3, 1, 0),
    SPI1_MISO_PB4 =     PINAF(PB4, 1, 0),
    SPI1_MOSI_PB5 =     PINAF(PB5, 1, 0),    
#undef _CURRENT_AF    
    
    // Alternate function 6
#define _CURRENT_AF     6
    DECL_PIN            (TIM, 1, BKIN, PA6),
    DECL_PIN            (TIM, 1, CH, 1, N, PA7),
    DECL_PIN            (TIM, 1, CH, 1, PA8),
    DECL_PIN            (TIM, 1, CH, 2, PA9),
    DECL_PIN            (TIM, 1, CH, 3, PA10),
    DECL_PIN            (TIM, 1, CH, 1, N, PA11),
    DECL_PIN            (TIM, 1, CH, 2, N, PA12),
    DECL_PIN            (TIM, 1, BKIN, PA14),
    DECL_PIN            (TIM, 1, CH, 2, N, PB0),
    DECL_PIN            (TIM, 1, CH, 3, N, PB1),
    DECL_PIN            (IR, OUT, PB9),
    DECL_PIN            (TIM, 1, BKIN, PB12),
    DECL_PIN            (TIM, 1, CH, 1, N, PB13),
    DECL_PIN            (TIM, 1, CH, 2, N, PB14),
    DECL_PIN            (TIM, 1, BKIN2, PC3), 
    DECL_PIN            (TIM, 1, CH, 3, N, PF0),   
#undef _CURRENT_AF
    
        // Alternate function 7
#define _CURRENT_AF     7
    DECL_PIN            (USART, 2, CTS, PA0),
    DECL_PIN            (USART, 2, RTS_DE, PA1),
    DECL_PIN            (USART, 2, TX, PA2),
    DECL_PIN            (USART, 2, RX, PA3),
    DECL_PIN            (USART, 2, CK, PA4),
    DECL_PIN            (USART, 1, CK, PA8),
    DECL_PIN            (USART, 1, TX, PA9),
    DECL_PIN            (USART, 1, RX, PA10),
    DECL_PIN            (USART, 1, CTS, PA11),
    DECL_PIN            (USART, 1, RTS_DE, PA12),
    DECL_PIN            (USART, 3, CTS, PA13),
    DECL_PIN            (USART, 2, TX, PA14),
    DECL_PIN            (USART, 2, RX, PA15),
    DECL_PIN            (USART, 2, TX, PB3),
    DECL_PIN            (USART, 2, RX, PB4),
    DECL_PIN            (USART, 2, CK, PB5),
    DECL_PIN            (USART, 1, TX, PB6),
    DECL_PIN            (USART, 1, RX, PB7),
    DECL_PIN            (USART, 3, RX, PB8),
    DECL_PIN            (USART, 3, TX, PB9),
    DECL_PIN            (USART, 3, TX, PB10),
    DECL_PIN            (USART, 3, RX, PB11),
    DECL_PIN            (USART, 3, CK, PB12),
    DECL_PIN            (USART, 3, CTS, PB13),
    DECL_PIN            (USART, 3, RTS_DE, PB14),
    DECL_PIN            (USART, 1, TX, PC4),
    DECL_PIN            (USART, 1, RX, PC5),
    DECL_PIN            (COMP, 6, OUT, PC6),
    DECL_PIN            (USART, 3, TX, PC10),
    DECL_PIN            (USART, 3, RX, PC11),
    DECL_PIN            (USART, 3, CK, PC12),   
#undef _CURRENT_AF
    
    
        // Alternate function 8
#define _CURRENT_AF     8
    DECL_PIN            (COMP, 2, OUT, PA2),  
    DECL_PIN            (COMP, 6, OUT, PA10),   
    DECL_PIN            (COMP, 2, OUT, PA12),
    DECL_PIN            (COMP, 4, OUT, PB1),  
    DECL_PIN            (COMP, 2, OUT, PB9),   
#undef _CURRENT_AF
    
       // Alternate function 9
#define _CURRENT_AF     9   
    DECL_PIN            (TIM, 15, CH, 1, N, PA1),
    DECL_PIN            (TIM, 15, CH, 1, PA2),
    DECL_PIN            (TIM, 15, CH, 2, PA3),
    DECL_PIN            (TIM, 15, BKIN, PA9),
    DECL_PIN            (CAN, 1, RX, PA11),
    DECL_PIN            (CAN, 1, TX, PA12),
    DECL_PIN            (TIM, 1, BKIN, PA15),  
    DECL_PIN            (CAN, 1, RX, PB8),
    DECL_PIN            (CAN, 1, TX, PB9),   
#undef _CURRENT_AF
    
        // Alternate function 10
#define _CURRENT_AF     10  
    DECL_PIN            (TIM, 2, CH, 3, PA9),
    DECL_PIN            (TIM, 2, CH, 4, PA10),  
    DECL_PIN            (TIM, 3, ETR, PB3),
    DECL_PIN            (TIM, 17, BKIN, PB4),
    DECL_PIN            (TIM, 17, CH, 1, PB5),
    DECL_PIN            (TIM, 3, CH, 4, PB7),   
#undef _CURRENT_AF
    
      // Alternate function 11
#define _CURRENT_AF     11  
    DECL_PIN            (TIM, 1, CH, 4, PA11),
    DECL_PIN            (TIM, 1, ETR, PA12),   
#undef _CURRENT_AF  
    
        // Alternate function 12
#define _CURRENT_AF     12 
    DECL_PIN            (TIM, 1, BKIN2, PA11),  
    DECL_PIN            (TIM, 1, BKIN, PB8),
   #undef _CURRENT_AF

#endif
    
};