#ifndef _CPUID_H
#define _CPUID_H

#include "stm32.h"

class CpuId
{
private:
    static const unsigned long * const mSignature;
    static const unsigned short * const mFlashSize;
    static const unsigned short * const mPackage;
    static const unsigned long * const mCpuId;

public:
    static unsigned long signature(int part);
    static unsigned long serial();
    static unsigned short flashSizeK();
    static constexpr uint32_t flashBase() {return FLASH_BASE;}
    static uint32_t flashEnd();
    static unsigned short packageId();
    static unsigned short deviceId();
    static unsigned short revId();
    static const char *name();
    static char revision();
    static unsigned long maxSysClk();
    static const char *package();
};

#endif
