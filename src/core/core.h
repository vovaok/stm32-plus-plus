#ifndef _CORE_H
#define _CORE_H

#include "coreexception.h"
#include "macros.h"

#if defined(__ICCARM__)
#include <intrinsics.h>
#endif
// NDEBUG is IAR predefined symbol in Release configuration
#ifndef NDEBUG
#include "qdebug.h"
#endif

#ifndef NO_CORE_SUPPORT
    #include "coretypes.h"
    #include "closure.h"
    #include "bytearray.h"
    #include "application.h"
#endif

#define foreach(type, var, container) for (type##::iterator __it__=container.begin(), type var=*it; __it__!=container.end(); ++__it__, var=*it)

unsigned long upper_power_of_two(unsigned long v);
//int log2i(uint32_t value);

#if defined(__ICCARM__)
// inverse of operator <<
#define log2i(x)    (31 - __iar_builtin_CLZ(x))
#endif

constexpr int operator ""_kHz(unsigned long long int value)
{
    return value * 1000;
}

constexpr int operator ""_MHz(unsigned long long int value)
{
    return value * 1000000;
}

#ifdef NO_RTTI
#define dynamic_cast reinterpret_cast
#endif

#endif
