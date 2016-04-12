#ifndef _CORE_H
#define _CORE_H

#include "coretypes.h"
#include "coreexception.h"
#include "closure.h"
#include "bytearray.h"
#include "application.h"

#define foreach(type, var, container) for (type##::iterator __it__=container.begin(), type var=*it; __it__!=container.end(); ++__it__, var=*it)

unsigned long upper_power_of_two(unsigned long v);

#endif
