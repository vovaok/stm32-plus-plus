#ifndef _FLASH4XX_H
#define _FLASH4XX_H

#include "stm32.h"
#include "cpuid.h"
#include "core/coreexception.h"

class Flash
{
public:
    typedef enum
    {
        Sector0  = 0x0000,
        Sector1  = 0x0008,
        Sector2  = 0x0010,
        Sector3  = 0x0018,
        Sector4  = 0x0020,
        Sector5  = 0x0028,
        Sector6  = 0x0030,
        Sector7  = 0x0038,
        Sector8  = 0x0040,
        Sector9  = 0x0048,
        Sector10 = 0x0050,
        Sector11 = 0x0058,
        Sector12 = 0x0080,
        Sector13 = 0x0088,
        Sector14 = 0x0090,
        Sector15 = 0x0098,
        Sector16 = 0x00A0,
        Sector17 = 0x00A8,
        Sector18 = 0x00B0,
        Sector19 = 0x00B8,
        Sector20 = 0x00C0,
        Sector21 = 0x00C8,
        Sector22 = 0x00D0,
        Sector23 = 0x00D8,
        InvalidSector = 0xFFFF
    } Sector;

    typedef enum
    {
        sCompleted          = 0,
        sBusy               = 0x00010000,
        sErrorReadProtect   = 0x00000100,
        sErrorProgSeq       = 0x00000080,
        sErrorParallelism   = 0x00000040,
        sErrorAlignment     = 0x00000020,
        sErrorWriteProtect  = 0x00000010,
        StatusMask          = 0x000101F0
    } Status;

private:
    static const int mAddresses[25];

    static Status status();
    static Status wait();

public:
    static Sector getSectorByIdx(unsigned char index);
    static unsigned char getIdxOfSector(Sector sector);
    static Sector getSectorByAddress(unsigned long addr);
    static unsigned long getBeginOfSector(Sector sector);
    static unsigned long getSizeOfSector(Sector sector);
    static bool isSectorValid(Sector sector);
    static Sector lastSector();

    static void unlock();
    static void lock();

    static Status eraseSector(Sector sector);
    static uint32_t readWord(uint32_t address) {return *reinterpret_cast<uint32_t*>(address);}
    static Status programWord(unsigned long address, unsigned long value);
    static Status programData(unsigned long address, const void *data, unsigned long size);
    static Status programDataInverted(unsigned long address, const void *data, unsigned long size);
};

#endif
