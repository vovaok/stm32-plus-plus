#ifndef _CORE_H
#define _CORE_H

#include "coreexception.h"
#include "macros.h"

#if defined(__ICCARM__)
#include <intrinsics.h>
#elif defined(__GNUC__)
#include <cmsis_gcc.h>

typedef uint32_t __istate_t;

__istate_t __get_interrupt_state() __attribute__((alias(__get_PRIMASK)));
void __set_interrupt_state(__istate_t) __attribute__((alias(__set_PRIMASK)));
void __enable_interrupt() __attribute__((alias(__enable_irq)));
void __disable_interrupt() __attribute__((alias(__disable_irq)));
#endif

// NDEBUG is IAR predefined symbol in Release configuration
#ifndef NDEBUG
#include "qdebug.h"
#endif

#ifndef NO_CORE_SUPPORT
    #include "coretypes.h"
    #include "closure.h"
    #include "bytearray.h"
//    #include "application.h"
#endif

#define foreach(type, var, container) for (type##::iterator __it__=container.begin(), type var=*it; __it__!=container.end(); ++__it__, var=*it)

unsigned long upper_power_of_two(unsigned long v);
//int log2i(uint32_t value);

// inverse of operator <<
#if defined(__ICCARM__)
#define log2i(x)    (31 - __iar_builtin_CLZ(x))
#elif defined(__GNUC__)
#define log2i(x)    (31 - __builtin_clz(x))
#endif

constexpr int operator ""_k(unsigned long long int value)
{
    return value * 1000;
}

constexpr int operator ""_M(unsigned long long int value)
{
    return value * 1000000;
}

#ifdef NO_RTTI
#define dynamic_cast reinterpret_cast
#endif

#endif
