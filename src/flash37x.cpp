#include "flash37x.h"
#if defined(STM32F37X)

Flash::Sector Flash::getSectorByAddress(unsigned long addr)
{
  return static_cast<Flash::Sector>((addr - 0x08000000) >> 11);
}

unsigned long Flash::getBeginOfSector(Flash::Sector page)
{
    return 0x08000000 + (int)page * 0x0800;
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
    FLASH->CR = FLASH_CR_PER;
    FLASH->AR = getBeginOfSector(sector);
    FLASH->CR |= FLASH_CR_STRT;
    status = wait();
    FLASH->CR &= ~FLASH_CR_PER;
    FLASH->SR |= FLASH_SR_EOP; // clear EndOfOperation flag
    return status;
}

Flash::Status Flash::programHalfWord(unsigned long address, unsigned short value)
{
    Status status;
    status = wait();
    FLASH->CR |= FLASH_CR_PG;
    *reinterpret_cast<volatile unsigned short*>(address) = value;
    status = wait();
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->SR |= FLASH_SR_EOP; // clear EndOfOperation flag
    return status;
}

Flash::Status Flash::programWord(unsigned long address, unsigned long value)
{
    Status status;
    status = wait();
    FLASH->CR |= FLASH_CR_PG;
    reinterpret_cast<volatile unsigned short*>(address)[0] = value & 0xFFFF;
    reinterpret_cast<volatile unsigned short*>(address)[1] = value >> 16;
    status = wait();
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->SR |= FLASH_SR_EOP; // clear EndOfOperation flag
    return status;
}

Flash::Status Flash::programData(unsigned long address, const void *data, unsigned long size)
{
    Status status;
    status = wait();
    FLASH->CR |= FLASH_CR_PG;
    int sz = (size + 1) / 2;
    for (unsigned long i=0; i<sz; i++)
    {
        reinterpret_cast<volatile unsigned short*>(address)[i] = reinterpret_cast<const unsigned short*>(data)[i];
        status = wait();
    }
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->SR |= FLASH_SR_EOP; // clear EndOfOperation flag
    return status;
}

#endif