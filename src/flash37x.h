#ifndef _FLASH37X_H
#define _FLASH37X_H
#if defined(STM32F37X)

#include "stm32.h"

// Sector is Page

class Flash
{
public:
    typedef enum
    {
        Page0 = 0,
        Page1 = 1,
        // ...
        Page127 = 127,
        PageInvalid = 0xFF
    } Sector;
  
    typedef enum
    {
        sBusy               = 0x00000001,
        sErrorProgramming   = 0x00000004,
        sErrorWriteProtect  = 0x00000010,
        sEndOfOperation     = 0x00000020,
        StatusMask          = 0x00000035
    } Status;
    
private:
    static Status status();
    static Status wait();
    
public:
    static Sector getSectorByAddress(unsigned long addr);
    static unsigned long getBeginOfSector(Sector page);
    static inline unsigned long getSizeOfSector(Sector page) {return 0x800;} // page size = 2K
  
    static void unlock();
    static void lock();
    
    static Status eraseSector(Sector sector);
    
    // The Flash memory interface preliminarily reads the value at the addressed main Flash 
    // memory location and checks that it has been erased. If not, the program operation is 
    // skipped and a warning is issued by the PGERR bit in FLASH_SR register
    
    static Status programHalfWord(unsigned long address, unsigned short value);
    static Status programWord(unsigned long address, unsigned long value);
    static Status programData(unsigned long address, const void *data, unsigned long size);
};

#endif
#endif