#include "fmc.h"

void SDRAM::initialize(int bank)
{
    Fmc fmc;
    // initialize periph
    fmc.init(this, bank);

    // enable clock
    fmc.sdramCmd(bank, Fmc::CmdModeClkEnabled);

     // delay at least 100us
    for (int w=100000; --w;);

    // issue precharge all
    fmc.sdramCmd(bank, Fmc::CmdModePALL);

    // perform at least 2 autorefresh cycles
    fmc.sdramCmd(bank, Fmc::CmdModeAutoRefresh, 4); //!< @todo how many autorefresh cycles?

    // program the mode register
    uint32_t modereg = BurstLength2 | BurstTypeSequential;
    if (CASLatency == 2)
        modereg |= CASLatency2;
    else
        modereg |= CASLatency3;
    if (!burstEnabled)
        modereg |= WriteBurstModeSingle;
    fmc.sdramCmd(bank, Fmc::CmdModeLoadMode, modereg);
}

int SDRAM::memorySize() const
{
    return banks * rows * columns * (busWidth / 8);
}

// SDRAM support
#if defined(FMC_Bank5_6)

void Fmc::init(SDRAM *sdram, int bank)
{
    if (bank != 1 && bank != 2)
        THROW(Exception::InvalidPeriph);
    if (sdram->busWidth != 8 && sdram->busWidth != 16 && sdram->busWidth != 32)
        THROW(Exception::OutOfRange);
    if (sdram->rows < 11 || sdram->rows > 13 || sdram->columns < 8 || sdram->columns > 11)
        THROW(Exception::OutOfRange);
    if (sdram->banks != 2 && sdram->banks != 4)
        THROW(Exception::OutOfRange);
    if (sdram->CASLatency < 1 || sdram->CASLatency > 3)
        THROW(Exception::OutOfRange);

    initAddressPins(sdram->rows); // assume rows > columns
//    initAddressPins(MAX(sdram->rows, sdram->columns));

    initDataPins(sdram->busWidth);

    //! @todo check bank pins dependencies
    // init bank select pins:
    Gpio::config(sdram->banks >> 1, Gpio::FMC_SDRAM_BA0, Gpio::FMC_SDRAM_BA1);
    // init output byte mask pins (it seems to be matched with bus width)
    Gpio::config(sdram->busWidth >> 3,
                 Gpio::FMC_NBL0, Gpio::FMC_NBL1, Gpio::FMC_NBL2, Gpio::FMC_NBL3);
    // init clock and strobe pins:
    Gpio::config(3, Gpio::FMC_SDCLK, Gpio::FMC_SDNRAS, Gpio::FMC_SDNCAS);

    //! @todo provide remap functionality
    // init remappable write enable pin:
    Gpio::config(Gpio::FMC_SDNWE_PC0);
//    Gpio::config(Gpio::FMC_SDNWE_PH5);

    if (bank == 1)
    {
        Gpio::config(2, Gpio::FMC_SDCKE0_PC3, Gpio::FMC_SDNE0_PC2);
//        Gpio::config(2, Gpio::FMC_SDCKE0_PH2, Gpio::FMC_SDNE0_PH3);
    }
    else
    {
        Gpio::config(2, Gpio::FMC_SDCKE1_PB5, Gpio::FMC_SDNE1_PB6);
//        Gpio::config(2, Gpio::FMC_SDCKE1_PH7, Gpio::FMC_SDNE1_PH6);
    }

    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;

    bool writeProtect = false;

    // calculate real frequency and prescaler
    int freq = sdram->frequency_MHz;
    int hclk = rcc().hClk() / 1000000;
    int sdclk = (hclk - 1) / freq + 1;
    if (sdclk < 2 || sdclk > 3)
        THROW(Exception::OutOfRange);
    freq = hclk / sdclk; // real freq
    sdram->frequency_MHz = freq;

    //! @todo find out how to set this parameter
    int rpipe = 0;//1;

    // fill control register:
    uint32_t sdcr = (sdram->columns - 8) |
                    ((sdram->rows - 11) << 2) |
                    ((sdram->busWidth >> 4) << 4) |
                    ((sdram->banks >> 2) << 6) |
                    (sdram->CASLatency << 7) |
                    (writeProtect? (1 << 9): 0) |
                    (sdclk << 10) |
                    (sdram->burstEnabled? (1 << 12): 0) |
                    (rpipe << 13);
    uint32_t sdcr_mask = (3 << 10) | (1 << 12) | (3 << 13);
    // common settings for all banks:
//    FMC_Bank5_6->SDCR[0] = (FMC_Bank5_6->SDCR[0] & ~sdcr_mask) | (sdcr & sdcr_mask);
    FMC_Bank5_6->SDCR[0] = (sdcr & sdcr_mask);
    // dedicated settings for the bank:
    FMC_Bank5_6->SDCR[bank - 1] = sdcr;

    // fill timing register
    uint32_t sdtr = ((sdram->tMRD - 1) <<  0) |
                    ((sdram->tXSR - 1) <<  4) |
                    ((sdram->tRAS - 1) <<  8) |
                    ((sdram->tRC  - 1) << 12) |
                    ((sdram->tWR  - 1) << 16) |
                    ((sdram->tRP  - 1) << 20) |
                    ((sdram->tRCD - 1) << 24);
    uint32_t sdtr_mask = (0xF << 12) | (0xF << 16) | (0xF << 20);
    // common settings for all banks:
//    FMC_Bank5_6->SDTR[0] = (FMC_Bank5_6->SDTR[0] & ~sdtr_mask) | (sdtr & sdtr_mask);
    FMC_Bank5_6->SDTR[0] = (sdtr & sdtr_mask);
    // dedicated settings for the bank:
    FMC_Bank5_6->SDTR[bank - 1] = sdtr;

    // calculate refresh rate according to the STM32 datasheet
    int refreshRate = sdram->refreshPeriod_ms * 1000 * freq / (1 << sdram->rows) - 20;
    if (refreshRate < 41 || refreshRate >= (1<<13))
        THROW(Exception::OutOfRange);
    FMC_Bank5_6->SDRTR = refreshRate << 1;
}

void Fmc::sdramCmd(int bank, SdramCmdMode cmd, int value)
{
    bank = (bank << 1) | (bank >> 1); // swap bank bits, thank you f***ing STM!
    uint32_t sdcmr = (cmd & 7) | ((bank & 3) << 3);
    if (cmd == CmdModeAutoRefresh)
        sdcmr |= ((value - 1) & 0xF) << 5;
    if (cmd == CmdModeLoadMode)
        sdcmr |= (value & 0x1FFF) << 9;

    // issue a command mode request
    FMC_Bank5_6->SDCMR = sdcmr;

    // wait for request complete
    while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
}

#endif

void Fmc::initAddressPins(int width)
{
    Gpio::config(width,
        Gpio::FMC_A0,  Gpio::FMC_A1,  Gpio::FMC_A2,  Gpio::FMC_A3,  Gpio::FMC_A4,  Gpio::FMC_A5,  Gpio::FMC_A6,  Gpio::FMC_A7,
        Gpio::FMC_A8,  Gpio::FMC_A9,  Gpio::FMC_A10, Gpio::FMC_A11, Gpio::FMC_A12, Gpio::FMC_A13, Gpio::FMC_A14, Gpio::FMC_A15,
        Gpio::FMC_A16, Gpio::FMC_A17, Gpio::FMC_A18, Gpio::FMC_A19, Gpio::FMC_A20, Gpio::FMC_A21, Gpio::FMC_A22, Gpio::FMC_A23,
        Gpio::FMC_A24, Gpio::FMC_A25);
}

void Fmc::initDataPins(int width)
{
    Gpio::config(width,
        Gpio::FMC_D0,  Gpio::FMC_D1,  Gpio::FMC_D2,  Gpio::FMC_D3,  Gpio::FMC_D4,  Gpio::FMC_D5,  Gpio::FMC_D6,  Gpio::FMC_D7,
        Gpio::FMC_D8,  Gpio::FMC_D9,  Gpio::FMC_D10, Gpio::FMC_D11, Gpio::FMC_D12, Gpio::FMC_D13, Gpio::FMC_D14, Gpio::FMC_D15,
        Gpio::FMC_D16, Gpio::FMC_D17, Gpio::FMC_D18, Gpio::FMC_D19, Gpio::FMC_D20, Gpio::FMC_D21, Gpio::FMC_D22, Gpio::FMC_D23,
        Gpio::FMC_D24, Gpio::FMC_D25, Gpio::FMC_D26, Gpio::FMC_D27, Gpio::FMC_D28, Gpio::FMC_D29, Gpio::FMC_D30, Gpio::FMC_D31);
}
