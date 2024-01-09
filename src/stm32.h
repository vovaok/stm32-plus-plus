#ifndef _STM32_H
#define _STM32_H

#if defined (STM32F405xx) || defined (STM32F415xx) || defined (STM32F407xx) || defined (STM32F417xx) || \
    defined (STM32F427xx) || defined (STM32F437xx) || defined (STM32F429xx) || defined (STM32F439xx) || \
    defined (STM32F401xC) || defined (STM32F401xE) || defined (STM32F410Tx) || defined (STM32F410Cx) || \
    defined (STM32F410Rx) || defined (STM32F411xE) || defined (STM32F446xx) || defined (STM32F469xx) || \
    defined (STM32F479xx) || defined (STM32F412Cx) || defined (STM32F412Rx) || defined (STM32F412Vx) || \
    defined (STM32F412Zx) || defined (STM32F413xx) || defined (STM32F423xx)

#include "../lib/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"

#elif defined (STM32L412xx) || defined (STM32L422xx) || \
      defined (STM32L431xx) || defined (STM32L432xx) || defined (STM32L433xx) || defined (STM32L442xx) || defined (STM32L443xx) || \
      defined (STM32L451xx) || defined (STM32L452xx) || defined (STM32L462xx) || \
      defined (STM32L471xx) || defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) || \
      defined (STM32L496xx) || defined (STM32L4A6xx) || \
      defined (STM32L4P5xx) || defined (STM32L4Q5xx) || \
      defined (STM32L4R5xx) || defined (STM32L4R7xx) || defined (STM32L4R9xx) || defined (STM32L4S5xx) || defined (STM32L4S7xx) || defined (STM32L4S9xx)

#include "../lib/CMSIS/Device/ST/STM32L4xx/Include/stm32l4xx.h"

#elif defined (STM32G431xx) || defined (STM32G441xx) || defined (STM32G471xx) || \
      defined (STM32G473xx) || defined (STM32G474xx) || defined (STM32G484xx) || \
      defined (STM32GBK1CB) || defined (STM32G491xx) || defined (STM32G4A1xx)

#include "../lib/CMSIS/Device/ST/STM32G4xx/Include/stm32g4xx.h"        
        
#elif defined (STM32F301x8) || defined (STM32F302x8) || defined (STM32F318xx) || \
      defined (STM32F302xC) || defined (STM32F303xC) || defined (STM32F358xx) || \
      defined (STM32F303x8) || defined (STM32F334x8) || defined (STM32F328xx) || \
      defined (STM32F302xE) || defined (STM32F303xE) || defined (STM32F398xx) || \
      defined (STM32F373xC) || defined (STM32F378xx)

#include "../lib/CMSIS/Device/ST/cmsis_device_f3/Include/stm32f3xx.h"        
        
#endif

#endif