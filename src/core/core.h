#ifndef _CORE_H
#define _CORE_H

#include "coreexception.h"
#include "macros.h"

#include <intrinsics.h>
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

#ifdef NO_RTTI
#define dynamic_cast reinterpret_cast
#endif

#endif
