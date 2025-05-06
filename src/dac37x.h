#ifndef _DAC37X_H
#define _DAC37X_H

#include "stm32.h"
#include "gpio.h"
#include "dma.h"

class Dac
{
public:
  
  
#if defined(STM32F303x8) || defined(STM32F328xx)  || defined(STM32F303xC)
  typedef enum
    {
        TriggerNone =     0,
        TriggerTim2 =     (DAC_CR_TSEL1_2                                   | DAC_CR_TEN1),       
        TriggerTim15 =    (                 DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0 | DAC_CR_TEN1),
        TriggerTim6 =     (                                                   DAC_CR_TEN1),
        TriggerTim7 =     (                 DAC_CR_TSEL1_1                  | DAC_CR_TEN1),
        TriggerTim3 =     (                                  DAC_CR_TSEL1_0 | DAC_CR_TEN1),
        TriggerExtPin =   (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1                  | DAC_CR_TEN1),
        TriggerSoftware = (DAC_CR_TSEL1                                     | DAC_CR_TEN1)
    } Trigger;
#else
    typedef enum
    {
        TriggerNone = DAC_Trigger_None,
        TriggerTim2 = DAC_Trigger_T2_TRGO,
        TriggerTim3 = DAC_Trigger_T3_TRGO,
        TriggerTim4 = DAC_Trigger_T4_TRGO,
        TriggerTim5 = DAC_Trigger_T5_TRGO,
        TriggerTim18 = DAC_Trigger_T18_TRGO,
        TriggerTim6 = DAC_Trigger_T6_TRGO,
        TriggerTim7 = DAC_Trigger_T7_TRGO,
        TriggerExtPin = DAC_Trigger_Ext_IT9,
        TriggerSoftware = DAC_Trigger_Software
    } Trigger;
#endif
    typedef enum
    {
        Res8bit,
        Res12bit,
        Res16bit
    } Resolution;
  
private:
    DAC_TypeDef *mDac;
    enum {ChannelNone=0, Channel1=1, Channel2=2, ChannelBoth=3} mChannels;
    Resolution mResolution;
    bool mEnabled;
    volatile unsigned int *mData;
  
public:
    Dac(Gpio::Config out1, Gpio::Config out2 = Gpio::NoConfig);
    ~Dac();
    
    void setResolution(Resolution resolution);
    void selectTrigger(Trigger trigger);
    inline void setValue(unsigned short value) {*mData = value;}
    void setValue(unsigned short value1, unsigned short value2);
    void setEnabled(bool enabled);
    bool isEnabled() const {return mEnabled;}
    
    void configDma(Dma *dma);
};
#endif