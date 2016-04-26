#ifndef _COREEXCEPTION_H
#define _COREEXCEPTION_H

#include <exception>

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
