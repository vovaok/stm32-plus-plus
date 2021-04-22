#ifndef __STM32F2xx_CONF_H
#define __STM32F2xx_CONF_H

/*! System clock.
    Defines System timer interrupts period.
*/
#define SYSTEM_CLOCK_MS             1

/* Includes ------------------------------------------------------------------*/
#if defined(STM32F2XX)
    #include "stm32f2xx.h"
    #include "stm32f2xx_adc.h"
    #include "stm32f2xx_can.h"
    #include "stm32f2xx_crc.h"
    #include "stm32f2xx_cryp.h"
    #include "stm32f2xx_dac.h"
    #include "stm32f2xx_dbgmcu.h"
    #include "stm32f2xx_dcmi.h"
    #include "stm32f2xx_dma.h"
    #include "stm32f2xx_exti.h"
    #include "stm32f2xx_flash.h"
    #include "stm32f2xx_fsmc.h"
    #include "stm32f2xx_hash.h"
    #include "stm32f2xx_gpio.h"
    #include "stm32f2xx_i2c.h"
    #include "stm32f2xx_iwdg.h"
    #include "stm32f2xx_pwr.h"
    #include "stm32f2xx_rcc.h"
    #include "stm32f2xx_rng.h"
    #include "stm32f2xx_rtc.h"
    #include "stm32f2xx_sdio.h"
    #include "stm32f2xx_spi.h"
    #include "stm32f2xx_syscfg.h"
    #include "stm32f2xx_tim.h"
    #include "stm32f2xx_usart.h"
    #include "stm32f2xx_wwdg.h"
    #include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */
#elif defined(STM32F37X)
	#include "stm32f37x_adc.h"
	#include "stm32f37x_can.h"
	#include "stm32f37x_cec.h"
	#include "stm32f37x_crc.h"
	#include "stm32f37x_comp.h"
	#include "stm32f37x_dac.h"
	#include "stm32f37x_dbgmcu.h"
	#include "stm32f37x_dma.h"
	#include "stm32f37x_exti.h"
	#include "stm32f37x_flash.h"
	#include "stm32f37x_gpio.h"
	#include "stm32f37x_syscfg.h"
	#include "stm32f37x_i2c.h"
	#include "stm32f37x_iwdg.h"
	#include "stm32f37x_pwr.h"
	#include "stm32f37x_rcc.h"
	#include "stm32f37x_rtc.h"
	#include "stm32f37x_sdadc.h"
	#include "stm32f37x_spi.h"
	#include "stm32f37x_tim.h"
	#include "stm32f37x_usart.h"
	#include "stm32f37x_wwdg.h"
	#include "stm32f37x_misc.h"
#elif defined(STM32F4XX) | defined(STM32F40XX)
    #include "stm32f4xx.h"
    #include "stm32f4xx_adc.h"
    #include "stm32f4xx_can.h"
    #include "stm32f4xx_crc.h"
    #include "stm32f4xx_cryp.h"
    #include "stm32f4xx_dac.h"
    #include "stm32f4xx_dbgmcu.h"
    #include "stm32f4xx_dcmi.h"
    #include "stm32f4xx_dma.h"
    #include "stm32f4xx_exti.h"
    #include "stm32f4xx_flash.h"
    #include "stm32f4xx_fsmc.h"
    #include "stm32f4xx_hash.h"
    #include "stm32f4xx_gpio.h"
    #include "stm32f4xx_i2c.h"
    #include "stm32f4xx_iwdg.h"
    #include "stm32f4xx_pwr.h"
    #include "stm32f4xx_rcc.h"
    #include "stm32f4xx_rng.h"
    #include "stm32f4xx_rtc.h"
    #include "stm32f4xx_sdio.h"
    #include "stm32f4xx_spi.h"
    #include "stm32f4xx_syscfg.h"
    #include "stm32f4xx_tim.h"
    #include "stm32f4xx_usart.h"
    #include "stm32f4xx_wwdg.h"
    #include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */
#elif defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F429xx)
    #include "stm32f4xx.h"
    #include "stm32f4xx_adc.h"
    #include "stm32f4xx_can.h"
    #include "stm32f4xx_crc.h"
    #include "stm32f4xx_cryp.h"
    #include "stm32f4xx_dac.h"
    #include "stm32f4xx_dbgmcu.h"
    #include "stm32f4xx_dcmi.h"
    #include "stm32f4xx_dma.h"
    #include "stm32f4xx_dma2d.h"
    #include "stm32f4xx_exti.h"
    #include "stm32f4xx_flash.h"
    #include "stm32f4xx_fmc.h"
    #include "stm32f4xx_fsmc.h"
    #include "stm32f4xx_hash.h"
    #include "stm32f4xx_gpio.h"
    #include "stm32f4xx_i2c.h"
    #include "stm32f4xx_iwdg.h"
    #include "stm32f4xx_ltdc.h"
    #include "stm32f4xx_pwr.h"
    #include "stm32f4xx_rcc.h"
    #include "stm32f4xx_rng.h"
    #include "stm32f4xx_rtc.h"
    #include "stm32f4xx_sai.h"
    #include "stm32f4xx_sdio.h"
    #include "stm32f4xx_spi.h"
    #include "stm32f4xx_syscfg.h"
    #include "stm32f4xx_tim.h"
    #include "stm32f4xx_usart.h"
    #include "stm32f4xx_wwdg.h"
    #include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */
#endif


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* If an external clock source is used, then the value of the following define 
   should be set to the value of the external clock source, else, if no external 
   clock is used, keep this define commented */
/*#define I2S_EXTERNAL_CLOCK_VAL   12288000 */ /* Value of the external clock in Hz */


/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#ifdef __GNUC__
#define __NO_INIT(definition) definition __attribute__((section (".noinit")))
#else
#define __NO_INIT(definition) __no_init definition
#endif

#endif
