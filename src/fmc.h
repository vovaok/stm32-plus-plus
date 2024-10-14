#pragma once

#include "gpio.h"
#include "rcc.h"
#include "core/core.h"

class SDRAM
{
public:
    int frequency_MHz = 100;
    int refreshPeriod_ms = 64;
    uint8_t rows = 11;
    uint8_t columns = 8;
    uint8_t banks = 4;
    uint8_t busWidth = 16;
    uint8_t CASLatency = 1;
    bool burstEnabled = false;
    // maybe make all timings represented in nanoseconds?
    uint8_t tMRD = 16;
    uint8_t tXSR = 16;
    uint8_t tRAS = 16;
    uint8_t tRC = 16;
    uint8_t tWR = 16;
    uint8_t tRP = 16;
    uint8_t tRCD = 16;

    void initialize(int bank);
    int memorySize() const;

//private:
    enum ModeReg
    {
        BurstLength1 = 0,
        BurstLength2 = 1,
        BurstLength4 = 2,
        BurstLength8 = 3,
        BurstLengthFullPage = 7,
        BurstTypeSequential = 0,
        BurstTypeInterleaved = (1<<3),
        CASLatency2 = (2<<4),
        CASLatency3 = (3<<4),
        WriteBurstModeProgrammed = (0<<9),
        WriteBurstModeSingle = (1<<9)
    };
};

class Fmc
{
public:
    void init(SDRAM *sdram, int bank);

//protected:
    friend class SDRAM;

    void initAddressPins(int width);
    void initDataPins(int width);

    typedef enum
    {
        CmdModeNormal = 0,
        CmdModeClkEnabled = 1,
        CmdModePALL = 2,
        CmdModeAutoRefresh = 3,
        CmdModeLoadMode = 4,
        CmdModeSelfRefresh = 5,
        CmdModePowerDown = 6
    } SdramCmdMode;

    void sdramCmd(int bank, SdramCmdMode cmd, int value = 0);
};