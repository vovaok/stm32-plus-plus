#ifndef _ADS1220_H
#define _ADS1220_H

#include "spi.h"

typedef enum
{
    ADS1220_POWERDOWN   = 0x02, //!< Enter power-down mode
    ADS1220_RESET       = 0x06, //!< Reset the device
    ADS1220_START       = 0x08, //!< Start or restart (synchronize) conversions
    ADS1220_RDATA       = 0x10, //!< Read data by command; supports multiple read back
    ADS1220_RREG        = 0x20, //!< Read nnnnn registers starting at address rrrrr
    ADS1220_WREG        = 0x40, //!< Write nnnnn registers starting at address rrrrr
} Ads1220_opcode;

typedef enum
{
    ADS1220_CONFIG0     = 0x00,
    ADS1220_CONFIG1     = 0x01,
    ADS1220_CONFIG2     = 0x02,
    ADS1220_CONFIG3     = 0x03,
} Ads1220_register;

typedef union
{
    unsigned long word;
    unsigned char reg[4];
    struct
    {
        // CONFIG0
        unsigned char PGA_BYPASS: 1;
        unsigned char GAIN: 3;
        unsigned char MUX: 4;
        // CONFIG1
        unsigned char BCS: 1;
        unsigned char TS: 1;
        unsigned char CM: 1;
        unsigned char MODE: 2;
        unsigned char DR: 3;
        // CONFIG2
        unsigned char IDAC: 3;
        unsigned char PSW: 1;
        unsigned char FILTER: 2;
        unsigned char VREF: 2;
        // CONFIG3
        unsigned char ZERO: 1;
        unsigned char DRDYM: 1;
        unsigned char I2MUX: 3;
        unsigned char I1MUX: 3;
    };
} Ads1220_conf;

typedef enum
{
    Ads1220_PGA_Enabled = 0,
    Ads1220_PGA_Disabled = 1
} Ads1220_PGA;

typedef enum
{
    Ads1220_Gain_1  = 0,
    Ads1220_Gain_2  = 1,
    Ads1220_Gain_4  = 2,
    Ads1220_Gain_8  = 3,
    Ads1220_Gain_16 = 4,
    Ads1220_Gain_32 = 5,
    Ads1220_Gain_64 = 6,
    Ads1220_Gain_128= 7,
} Ads1220_Gain;

typedef enum
{
    Ads1220_MUX_AIN0_AIN1   = 0,
    Ads1220_MUX_AIN0_AIN2   = 1,
    Ads1220_MUX_AIN0_AIN3   = 2,
    Ads1220_MUX_AIN1_AIN2   = 3,
    Ads1220_MUX_AIN1_AIN3   = 4,
    Ads1220_MUX_AIN2_AIN3   = 5,
    Ads1220_MUX_AIN1_AIN0   = 6,
    Ads1220_MUX_AIN3_AIN2   = 7,
    Ads1220_MUX_AIN0_AVSS   = 8,
    Ads1220_MUX_AIN1_AVSS   = 9,
    Ads1220_MUX_AIN2_AVSS   = 10,
    Ads1220_MUX_AIN3_AVSS   = 11,
    Ads1220_MUX_VREFP_VREFN = 12,
    Ads1220_MUX_AVDD_AVSS   = 13,
    Ads1220_MUX_SHORTED     = 14,
} Ads1220_MUX;

typedef enum
{
    Ads1220_BCS_Off = 0,
    Ads1220_BCS_On  = 1
} Ads1220_BCS;

typedef enum
{
    Ads1220_TS_Disabled = 0,
    Ads1220_TS_Enabled  = 1
} Ads1220_TS;

typedef enum
{
    Ads1220_CM_SingleShot = 0,
    Ads1220_CM_Continuous = 1
} Ads1220_CM;

typedef enum
{
    Ads1220_MODE_Normal     = 0,
    Ads1220_MODE_DutyCycle  = 1,
    Ads1220_MODE_Turbo      = 2
} Ads1220_MODE;

typedef enum
{
    Ads1220_DR_20_SPS   = 0,
    Ads1220_DR_45_SPS   = 1,
    Ads1220_DR_90_SPS   = 2,
    Ads1220_DR_175_SPS  = 3,
    Ads1220_DR_330_SPS  = 4,
    Ads1220_DR_600_SPS  = 5,
    Ads1220_DR_1000_SPS = 6
} Ads1220_DR;

typedef enum
{
    Ads1220_IDAC_Off    = 0,
    Ads1220_IDAC_10uA   = 1,
    Ads1220_IDAC_50uA   = 2,
    Ads1220_IDAC_100uA  = 3,
    Ads1220_IDAC_250uA  = 4,
    Ads1220_IDAC_500uA  = 5,
    Ads1220_IDAC_1000uA = 6,
    Ads1220_IDAC_1500uA = 7
} Ads1220_IDAC;

typedef enum
{
    Ads1220_PSW_AlwaysOpen = 0,
    Ads1220_PSW_AutoClose  = 1
} Ads1220_PSW;

typedef enum
{
    Ads1220_Filter_None     = 0,
    Ads1220_Filter_50_60Hz  = 1,
    Ads1220_Filter_50Hz     = 2,
    Ads1220_Filter_60Hz     = 3
} Ads1220_Filter;

typedef enum
{
    Ads1220_VREF_Internal    = 0,
    Ads1220_VREF_REFP0_REFN0 = 1,
    Ads1220_VREF_REFP1_REFN1 = 2,
    Ads1220_VREF_AVDD_AVSS   = 3
} Ads1220_VREF;

typedef enum
{
    Ads1220_DRDYM_DRDY      = 0,
    Ads1220_DRDYM_DOUT_DRDY = 1
} Ads1220_DRDYM;

typedef enum
{
    Ads1220_IDAC_Disabled  = 0,
    Ads1220_IDAC_AIN0      = 1,
    Ads1220_IDAC_AIN1      = 2,
    Ads1220_IDAC_AIN2      = 3,
    Ads1220_IDAC_AIN3      = 4,
    Ads1220_IDAC_REFP0     = 5,
    Ads1220_IDAC_REFN0     = 6,
} Ads1220_IxMUX;

class Ads1220
{
public:
    class Channel
    {
    public:
        long rawValue;
        long offset;
        float factor;
        float value;
        Channel() : rawValue(0), offset(0), factor(3300.0f), value(0.0f) {}
    };
  
private:
    Spi *mSpi;
    Gpio *mCsPin;
    Gpio *mResetPin;
    Gpio *mDrdyPin;
    Ads1220_conf mConf;
    bool mEnabled;
    
    Channel mChannels[15];
    Channel mTempChannel;
    
    void task();
    
    void select();
    void deselect();
    void sendOpcode(Ads1220_opcode cmd);
    void writeConfig(const Ads1220_conf &conf);
    unsigned char readReg(unsigned char address);
    void writeReg(unsigned char address, unsigned char data);
  
public:
    Ads1220(Spi *spi, Gpio::PinName csPin);
    void setResetPin(Gpio::PinName pin);
    void setDrdyPin(Gpio::PinName pin);

    void start();
    void stop();
    void start(unsigned char ch);
    void measureTemp();
    long readValue();
    
    Channel &channel(unsigned char ch) {return mChannels[ch];}
    
    long value(unsigned char ch) const {return mChannels[ch].rawValue;}
    long &valueRef(unsigned char ch) {return mChannels[ch].rawValue;}
    
    float &voltage(unsigned char ch) {return mChannels[ch].value;}
    float &temperature() {return mTempChannel.value;}
    
    bool isEnabled() const {return mEnabled;}
    bool isDataReady();
    
    const Ads1220_conf &config() const {return mConf;}
    void setConfig(const Ads1220_conf &conf);
};

#endif