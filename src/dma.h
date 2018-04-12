#ifndef _DMA_H
#define _DMA_H

#if defined(STM32F37X)
    #include "dma37x.h"
#else
    #include "dma4xx.h"
#endif


#endif