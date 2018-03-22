#ifndef _FLASH_H
#define _FLASH_H

#if defined(STM32F37X)
    #include "flash37x.h"
#else
    #include "flash4xx.h"
#endif

#endif
