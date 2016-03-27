#include "flash.h"

const int Flash::mAddresses[25] =
{
    0x08000000, 0x08004000, 0x08008000, 0x0800C000,
    0x08010000, 0x08020000, 0x08040000, 0x08060000,
    0x08080000, 0x080A0000, 0x080C0000, 0x080E0000,
    0x08100000, 0x08104000, 0x08108000, 0x0810C000,
    0x08110000, 0x08120000, 0x08140000, 0x08160000,
    0x08180000, 0x081A0000, 0x081C0000, 0x081E0000,
    0x08200000
};

Flash::Sector Flash::getSectorByIdx(unsigned char index)
{
    if (index < 12)
        return static_cast<Sector>(index << 3);
    else if (index < 24)
        return static_cast<Sector>(((index - 12) << 3) + 0x0080);
    return InvalidSector;
}

unsigned char Flash::getIdxOfSector(Sector sector)
{
    if (sector == InvalidSector)
        return 0xFF;
    unsigned short sec = static_cast<unsigned short>(sector);
    if (sec & 0x0080)
        return ((sec & 0x0078) >> 3) + 12;
    else
        return sec >> 3;
}

Flash::Sector Flash::getSectorByAddress(unsigned long addr)
{
    for (int i=0; i<25; i++)
    {
        if (addr < mAddresses[i])
            return getSectorByIdx(i-1);
    }
    return InvalidSector;
}

unsigned long Flash::getBeginOfSector(Sector sector)
{
    unsigned char idx = getIdxOfSector(sector);
    if (idx < 24)
        return mAddresses[idx];
    throw Exception::outOfRange;
}

unsigned long Flash::getSizeOfSector(Sector sector)
{
    unsigned char idx = getIdxOfSector(sector);
    if (idx < 24)
        return mAddresses[idx+1] - mAddresses[idx];
    throw Exception::outOfRange;
}

bool Flash::isSectorValid(Sector sector)
{
    unsigned long sz = CpuId::flashSizeK();
    unsigned long begin = getBeginOfSector(sector);
    return (begin < 0x08000000 + sz*0x400);      
}

Flash::Sector Flash::lastSector()
{
    unsigned long sz = CpuId::flashSizeK();
    return getSectorByAddress(0x08000000 + sz*0x400 - 1);
}
//---------------------------------------------------------------------------

Flash::Status Flash::status()
{
    return static_cast<Status>(FLASH->SR & StatusMask);
}

Flash::Status Flash::wait()
{
    while (FLASH->SR & sBusy);
    return static_cast<Status>(FLASH->SR & StatusMask);
}
//---------------------------------------------------------------------------

void Flash::unlock()
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}

void Flash::lock()
{
    FLASH->CR |= FLASH_CR_LOCK;
}
//---------------------------------------------------------------------------

Flash::Status Flash::eraseSector(Sector sector)
{
    Status status;
    status = wait();
    FLASH->CR = 0x00000200 | FLASH_CR_SER | (unsigned long)sector;
    FLASH->CR |= FLASH_CR_STRT;
    status = wait();
    FLASH->CR &= ~FLASH_CR_SER;
    return status;
}

Flash::Status Flash::programWord(unsigned long address, unsigned long value)
{
    Status status;
    status = wait();
    FLASH->CR = 0x00000200 | FLASH_CR_PG;
    *reinterpret_cast<volatile unsigned long*>(address) = value;
    status = wait();
    FLASH->CR &= ~FLASH_CR_PG;
    return status;
}

Flash::Status Flash::programData(unsigned long address, const void *data, unsigned long size)
{
    Status status;
    status = wait();
    FLASH->CR = 0x00000200 | FLASH_CR_PG;
    int sz = (size + 3) / 4;
    for (unsigned long i=0; i<sz; i++)
        reinterpret_cast<volatile unsigned long*>(address)[i] = reinterpret_cast<const unsigned long*>(data)[i];
    status = wait();
    FLASH->CR &= ~FLASH_CR_PG;
    return status;
}
