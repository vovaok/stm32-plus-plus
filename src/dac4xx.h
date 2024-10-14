#ifndef _DAC4XX_H
#define _DAC4XX_H

#include "gpio.h"
#include "dma.h"

class Dac
{
public:
    typedef enum
    {
        ChannelNone = 0,
        Channel1    = 1,
        Channel2    = 2,
        ChannelBoth = 3
    } Channels;
    
    typedef enum
    {
        Res8bit     = 0x10,
        Res12bit    = 0x20,
        Res16bit    = 0x30
    } Resolution;
    
    typedef enum
    {
        TriggerNone =     0,
        TriggerTim2 =     (DAC_CR_TSEL1_2                                   | DAC_CR_TEN1), 
        TriggerTim4 =     (DAC_CR_TSEL1_2                  | DAC_CR_TSEL1_0 | DAC_CR_TEN1),
        TriggerTim5 =     (                 DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0 | DAC_CR_TEN1),
        TriggerTim6 =     (                                                   DAC_CR_TEN1),
        TriggerTim7 =     (                 DAC_CR_TSEL1_1                  | DAC_CR_TEN1),
        TriggerTim8 =     (                                  DAC_CR_TSEL1_0 | DAC_CR_TEN1),
        TriggerExtPin =   (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1                  | DAC_CR_TEN1),
        TriggerSoftware = (DAC_CR_TSEL1                                     | DAC_CR_TEN1)
    } Trigger;
  
private:
    Channels mChannels;
//    DAC_InitTypeDef mConfig;
    Resolution mResolution;
//    uint32_t mAlign;
    __IO uint32_t *DR;
    bool mEnabled;
  
public:
    Dac(Channels channels, Resolution resolution);
    ~Dac();
    
    void selectTrigger(Trigger trigger);
    inline void setValue(unsigned short value) {*DR = value;}
    void setValue(unsigned short value1, unsigned short value2);
    void setEnabled(bool enabled);
    bool isEnabled() const {return mEnabled;}
    
    void disableBuffer();
    
    void configDma(Dma *dma);
};

#endif