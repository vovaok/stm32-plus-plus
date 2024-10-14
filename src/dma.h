#ifndef _DMA_H
#define _DMA_H

#if defined(STM32F37X)
    #include "dma37x.h"
#elif defined(STM32F3)
    #include "dma303x8.h"
#elif defined(STM32F4) || defined(STM32F7)
    #include "dma4xx.h"
#elif defined(STM32L4)
    #include "dma_l4.h"
#elif defined(STM32G4)
    #include "dma_l4.h"
#endif


#endif