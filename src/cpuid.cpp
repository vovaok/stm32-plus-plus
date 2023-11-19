#include "cpuid.h"

#if defined(STM32F37X)
const unsigned long * const  CpuId::mSignature = (const unsigned long*) 0x1FFFF7AC;
const unsigned short * const CpuId::mFlashSize = (const unsigned short*)0x1FFFF7CC;
const unsigned short * const CpuId::mPackage   = (const unsigned short*)0x1FFFF7AC; // signature instead of package
const unsigned long * const  CpuId::mCpuId     = (const unsigned long*) 0xE0042000;
#else
const unsigned long * const  CpuId::mSignature = (const unsigned long*) 0x1FFF7A10;
const unsigned short * const CpuId::mFlashSize = (const unsigned short*)0x1FFF7A22;
const unsigned short * const CpuId::mPackage   = (const unsigned short*)0x1FFF7BF0;
const unsigned long * const  CpuId::mCpuId     = (const unsigned long*) 0xE0042000;
#endif
//---------------------------------------------------------------------------

unsigned long CpuId::signature(int part)
{
    return mSignature[part];
}

unsigned long CpuId::serial()
{
    return mSignature[0] ^ mSignature[1] ^ mSignature[2];
}

unsigned short CpuId::flashSizeK()
{
    return *mFlashSize;
}

unsigned short CpuId::packageId()
{
    return (*mPackage) >> 8;
}

unsigned short CpuId::deviceId()
{
    return (*mCpuId) & 0xFFF;
}

unsigned short CpuId::revId()
{
    return ((*mCpuId) >> 16) & 0xF;
}

const char *CpuId::name()
{
    switch (deviceId())
    {
        case 0x413: return "STM32F40x_41x";
        case 0x419: return "STM32F42x_43x";
        case 0x421: return "STM32F446xx";
        case 0x423: return "STM32F401xB/C";
        case 0x431: return "STM32F411xC/E";
        case 0x432: return "STM32F37xxx";
        case 0x433: return "STM32F401xD/E";
        case 0x434: return "STM32F469_479";
        case 0x458: return "STM32F410";
#if defined(STM32F4)
        default:    return "STM32F4 family";
#elif defined(STM32L4)
        default:    return "STM32L4 family";
#elif defined(STM32G4)
        default:    return "STM32G4 family";
#endif
    }
}

char CpuId::revision()
{
    switch (revId())
    {
        case 0x1000: return 'A';
        case 0x2000: return 'B';
        case 0x1001: return 'Z';
        case 0x1003: return 'Y';
        default: return '?';
    }
}

unsigned long CpuId::maxSysClk()
{
    switch (deviceId())
    {
        case 0x413: return 168000000; // "STM32F40x_41x";
        case 0x419: return 180000000; // "STM32F42x_43x";
        case 0x421: return 180000000; // "STM32F446xx"
        case 0x423: return 84000000;  // "STM32F401xB/C";
        case 0x431: return 100000000; // "STM32F411xC/E";
        case 0x432: return 72000000;  // "STM32F37xxx"
        case 0x433: return 84000000;  // "STM32F401xD/E";
        case 0x434: return 180000000; // "STM32F469_479";
        case 0x458: return 100000000; // "STM32F410"

#if defined(STM32F4)
        default:    return 0;
#elif defined(STM32L4)
        default:    return 80000000;
#elif defined(STM32G4)
        default:    return 170000000;
#endif
    }
}

const char *CpuId::package()
{
    switch (deviceId())
    {
      case 0x458: // STM32F410
        switch (packageId())
        {
            case 0x0: return "WLCSP36";
            case 0x1: return "UFQFPN48";
            case 0x7: return "TQFP64";
            default: return "";
        }
      case 0x434: // STM32F469_479
        switch (packageId())
        {
            case 0x1: return "WLCSP168/UFBGA169";
            case 0x2: return "LQFP176/UFBGA176";
            case 0x4:
            case 0x5: return "LQFP208/TFBGA216";
            default: return "";
        }
      case 0x421: // STM32F446xx
        switch (packageId())
        {
            case 0x0: return "LQFP64";
            case 0x1: return "LQFP100";
            case 0x2: return "WLCSP81";
            case 0x3: return "LQFP144/UFBGA144";
            default: return "";
        }

      default: return "";
    }
}
