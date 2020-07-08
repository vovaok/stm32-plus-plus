#ifndef _COREEXCEPTION_H
#define _COREEXCEPTION_H

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

#ifndef NO_EXCEPTIONS
#include <exception>
#else
extern Exception::Reason gLastException;
#define throw gLastException=
#endif

#endif
