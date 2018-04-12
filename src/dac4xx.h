#ifndef _DAC4XX_H
#define _DAC4XX_H

#include "stm32_conf.h"
#include "dma.h"

class Dac
{
public:
    typedef enum {ChannelNone=0, Channel1=1, Channel2=2, ChannelBoth=3} Channels;
    typedef enum
    {
        TriggerNone = DAC_Trigger_None,
        TriggerTim2 = DAC_Trigger_T2_TRGO,
        TriggerTim4 = DAC_Trigger_T4_TRGO,
        TriggerTim5 = DAC_Trigger_T5_TRGO,
        TriggerTim6 = DAC_Trigger_T6_TRGO,
        TriggerTim7 = DAC_Trigger_T7_TRGO,
        TriggerTim8 = DAC_Trigger_T8_TRGO,
        TriggerExtPin = DAC_Trigger_Ext_IT9,
        TriggerSoftware = DAC_Trigger_Software
    } Trigger;
    typedef enum {Res8bit, Res12bit, Res16bit} Resolution;
    
    //typedef enum {}
  
private:
    Channels mChannels;
    DAC_InitTypeDef mConfig;
    Resolution mResolution;
    uint32_t mAlign;
    bool mEnabled;
  
public:
    Dac(Channels channels, Resolution resolution);
    ~Dac();
    
    void selectTrigger(Trigger trigger);
    void setValue(unsigned short value, unsigned short value2 = 0);
    void setEnabled(bool enabled);
    bool isEnabled() const {return mEnabled;}
    
    void configDma(Dma *dma);
};

#endif