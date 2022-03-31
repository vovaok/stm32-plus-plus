#ifndef _COREEXCEPTION_H
#define _COREEXCEPTION_H

namespace Exception
{

typedef enum
{
    Unknown,
    BadSoBad,
    InvalidPort,
    InvalidPin,
    InvalidPeriph,
    OutOfRange,
    ResourceBusy,
    
} Reason;

};

#ifdef __CPP_EXCEPTIONS__
    #include <exception>
    #define THROW(e) throw e
#else
    #define THROW(e) __asm volatile ("bkpt %0" : : "i"((int)e))
#endif

#endif
