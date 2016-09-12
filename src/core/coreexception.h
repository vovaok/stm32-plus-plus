#ifndef _COREEXCEPTION_H
#define _COREEXCEPTION_H

#ifndef NO_EXCEPTIONS
#include <exception>
#else
#define throw while(1);(void)
#endif

namespace Exception
{

typedef enum
{
    unknown,
    badSoBad,
    invalidPort,
    invalidPin,
    invalidPeriph,
    outOfRange,
    resourceBusy,
    
} Reason;

};

#endif
