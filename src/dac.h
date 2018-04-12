#ifndef _DAC_H
#define _DAC_H

#if defined(STM32F37X)
    #include "dac37x.h"
#else
    #include "dac4xx.h"
#endif

#endif
