#ifndef __CMSIS_ADAPTER
#define __CMSIS_ADAPTER

#include "stm32.h"

#if defined(__ICCARM__)
#elif defined(__GNUC__)
typedef uint32_t __istate_t;

#define __get_interrupt_state __get_PRIMASK
#define __set_interrupt_state __set_PRIMASK
#define __enable_interrupt    __enable_irq
#define __disable_interrupt   __disable_irq

#endif

#endif
