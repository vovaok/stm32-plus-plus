#pragma once

#include "spi.h"

class AD5412 {
public:
  
  union ControlRegister {
    uint32_t value;
    struct {
        uint32_t range : 3;       // R2, R1, R0 - output range selection
        uint32_t dcen : 1;        // Daisy chain enable
        uint32_t sren : 1;        // Slew rate enable
        uint32_t sr_step : 3;     // Slew rate step size
        uint32_t sr_clock : 4;    // Slew rate clock frequency
        uint32_t outen : 1;       // Output enable
        uint32_t rext : 1;        // External resistor select
        uint32_t ovrrng : 1;      // Overrange enable
        uint32_t clrsel : 1;      // Clear select
        uint32_t reserved : 16;   // Unused bits
    };
};
    enum OutputRange {
        RANGE_0_5V = 0b000,
        RANGE_0_10V = 0b001,
        RANGE_PM_5V = 0b010,
        RANGE_PM_10V = 0b011,
        RANGE_4_20MA = 0b101,
        RANGE_0_20MA = 0b110,
        RANGE_0_24MA = 0b111
    };
    
    enum SlewRateStep {
        STEP_1_16 = 0b000,   // AD5412: 1/16 LSB, AD5422: 1 LSB
        STEP_1_8 = 0b001,
        STEP_1_4 = 0b010,
        STEP_1_2 = 0b011,
        STEP_1 = 0b100,
        STEP_2 = 0b101,
        STEP_4 = 0b110,
        STEP_8 = 0b111
    };
    
    enum SlewRateClock {
    CLK_257KHZ = 0b0000,   // 257,730 Hz
    CLK_198KHZ = 0b0001,   // 198,410 Hz
    CLK_152KHZ = 0b0010,   // 152,440 Hz
    CLK_131KHZ = 0b0011,   // 131,580 Hz
    CLK_115KHZ = 0b0100,   // 115,740 Hz
    CLK_69KHZ  = 0b0101,   // 69,440 Hz
    CLK_37KHZ  = 0b0110,   // 37,590 Hz
    CLK_25KHZ  = 0b0111,   // 25,770 Hz
    CLK_20KHZ  = 0b1000,   // 20,160 Hz
    CLK_16KHZ  = 0b1001,   // 16,030 Hz
    CLK_10KHZ  = 0b1010,   // 10,290 Hz
    CLK_8KHZ   = 0b1011,   // 8,280 Hz
    CLK_6KHZ   = 0b1100,   // 6,900 Hz
    CLK_5KHZ   = 0b1101,   // 5,530 Hz
    CLK_4KHZ   = 0b1110,   // 4,240 Hz
    CLK_3KHZ   = 0b1111    // 3,300 Hz
};

     AD5412(Spi *spi, Gpio::PinName latchPin, Gpio::PinName clearPin = Gpio::noPin);
    ~AD5412();

    // Set output value (0-65535 for 16-bit, 0-4095 for 12-bit)
    void setValue(uint16_t value);

    // Configure output range and options
    void configure(OutputRange range, bool enableOutput = true, bool useExternalResistor = false);

    // Enable/disable slew rate control
    void setSlewRateControl(bool enable, SlewRateStep step = STEP_1, SlewRateClock clock = CLK_10KHZ);

    // Perform a reset (asynchronous clear)
    void reset(bool toMidscale = false);

    // Read status register
    uint8_t readStatus();
private:
    Spi *mSpi;
    Gpio *mLatchPin;
    Gpio *mClearPin;
    bool mEnabled;
    bool mIs16Bit = false; // true for AD5422 (16-bit), false for AD5412 (12-bit)

    void select();

    void deselect();

    uint32_t writeSPI(uint32_t data);

    void delayMicroseconds(uint32_t us);
};