#include "core.h"

#ifdef NO_EXCEPTIONS
__root Exception::Reason gLastException = Exception::Unknown;
#endif

unsigned long upper_power_of_two(unsigned long v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

//int log2i(uint32_t value)
//{
//    return 31 - __iar_builtin_CLZ(value);
//}