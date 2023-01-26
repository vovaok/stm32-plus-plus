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

#if defined(__CPP_EXCEPTIONS__) | defined(__CPP_Exceptions) | defined(__cpp_exceptions)
    #include <exception>
    #define THROW(e) do {__asm volatile ("bkpt %0" : : "i"((int)e)); throw e;} while (0)
#else
    #define THROW(e) __asm volatile ("bkpt %0" : : "i"((int)e))
#endif

#endif
