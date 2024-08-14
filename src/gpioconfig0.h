#pragma once
// this file is included by gpio.h, don't include it explicitly

enum Config
{
    NoConfig =          PINCONFIG(noPin, flagsDefault, afNone, 0x00), //!< empty configuration
  
     
    
      // ADC1
    DECL_ADC_PIN(1, 1, PA0),
    DECL_ADC_PIN(1, 2, PA1),
    DECL_ADC_PIN(1, 3, PA2),
    DECL_ADC_PIN(1, 4, PA3),
    DECL_ADC_PIN(1, 5, PA4),
    DECL_ADC_PIN(1, 6, PA5),
    DECL_ADC_PIN(1, 7, PA6),
    DECL_ADC_PIN(1, 8, PA7),  
    DECL_ADC_PIN(1, 9, PB0),
    DECL_ADC_PIN(1, 10, PB1),    
    
    
    #define _CURRENT_AF     0
    
    DECL_PIN            (SPI,  1, NSS,  PA4),  
    DECL_PIN            (SPI,  1, SCK,  PA5),   
    DECL_PIN            (SPI,  1, MISO, PA6),
    DECL_PIN            (SPI,  1, MOSI, PA7),  
    DECL_PIN            (TIM, 17, BKIN, PA10), 
    DECL_PIN            (SPI,  1, NSS,  PA15), 
    DECL_PIN            (SPI,  1, SCK,  PB3),   
    DECL_PIN            (SPI,  1, MISO, PB4),
    DECL_PIN            (SPI,  1, MOSI, PB5),  
    DECL_PIN            (USART,1, TX,   PB6),
    DECL_PIN            (USART,1, RX,   PB7),
    DECL_PIN            (SPI,  1, NSS,  PB12), 
    DECL_PIN            (SPI,  1, SCK,  PB13),   
    DECL_PIN            (SPI,  1, MISO, PB14),
    DECL_PIN            (SPI,  1, MOSI, PB15),  
    
    #undef _CURRENT_AF 
    
    #define _CURRENT_AF     1
    DECL_PIN            (USART,  1, CTS,  PA0),  
    DECL_PIN            (USART,  1, RTS,  PA1),   
    DECL_PIN            (USART,  1, RX,   PA2),
    DECL_PIN            (USART,  1, TX,   PA3),  
    DECL_PIN            (USART,  1, CK,   PA4),
    DECL_PIN            (TIM,    3, CH1,  PA6),
    DECL_PIN            (TIM,    3, CH2,  PA7), 
    DECL_PIN            (USART,  1, CK,   PA8),  
    DECL_PIN            (USART,  1, TX,   PA9),   
    DECL_PIN            (USART,  1, RX,   PA10),
    DECL_PIN            (USART,  1, CTS,  PA11),  
    DECL_PIN            (USART,  1, RTS,  PA12),
    DECL_PIN            (USART,  1, TX,   PA14),   
    DECL_PIN            (USART,  1, RX,   PA15),
    DECL_PIN            (TIM,    3, CH3,  PB0),
    DECL_PIN            (TIM,    3, CH4,  PB1), 
    DECL_PIN            (TIM,    3, CH1,  PB4),
    DECL_PIN            (TIM,    3, CH2,  PB5),
    DECL_PIN            (I2C,    1, SCL,  PB6),  
    DECL_PIN            (I2C,    1, SDA,  PB7),
    DECL_PIN            (I2C,    1, SCL,  PB8),  
    DECL_PIN            (I2C,    1, SDA,  PB9),
    DECL_PIN            (I2C,    1, SCL,  PB10),  
    DECL_PIN            (I2C,    1, SDA,  PB11),
    #undef _CURRENT_AF    
    
       // Alternate function 2
    #define _CURRENT_AF     2    
   
    DECL_PIN            (TIM,    1, BKIN, PA6),
    DECL_PIN            (TIM,    1, CH1N, PA7), 
    DECL_PIN            (TIM,    1, CH1,  PA8),  
    DECL_PIN            (TIM,    1, CH2,  PA9),   
    DECL_PIN            (TIM,    1, CH3,  PA10),
    DECL_PIN            (TIM,    1, CH4,  PA11),  
    DECL_PIN            (TIM,    1, ETR,  PA12),
    DECL_PIN            (TIM,    1, CH2N, PB0),
    DECL_PIN            (TIM,    1, CH3N, PB1), 
    DECL_PIN            (TIM,    16, BKIN,PB5),
    DECL_PIN            (TIM,    16, CH1N,PB6),  
    DECL_PIN            (TIM,    17, CH1N,PB7),
    DECL_PIN            (TIM,    16, CH1, PB8),  
    DECL_PIN            (TIM,    17, CH1, PB9),
    DECL_PIN            (TIM,    1, BKIN, PB12),  
    DECL_PIN            (TIM,    1, CH1N, PB13),
    DECL_PIN            (TIM,    1, CH2N, PB14),  
    DECL_PIN            (TIM,    1, CH3N, PB15),
   
#undef _CURRENT_AF    
 
     // Alternate function 3
#define _CURRENT_AF     3  
    DECL_PIN            (I2C,    1, SMBA, PB5),
  
#undef _CURRENT_AF    
    
  
        // Alternate function 4
#define _CURRENT_AF     4       
    DECL_PIN            (TIM,  14, CH1,   PA4),
    DECL_PIN            (TIM,  14, CH1,   PA7),
    DECL_PIN            (I2C,  1, SCL,    PA9),
    DECL_PIN            (I2C,  1, SDA,    PA10),
#undef _CURRENT_AF    
    
 


    
};