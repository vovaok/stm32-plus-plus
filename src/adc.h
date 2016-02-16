#ifndef _ADC_H
#define _ADC_H

#include "stm32_conf.h"
#include "dma.h"
#include "core/bytearray.h"
#include "gpio.h"

class Adc
{
public:
    typedef enum
    {
        Channel0 = 0, Channel1, Channel2, Channel3,
        Channel4, Channel5, Channel6, Channel7,
        Channel8, Channel9, Channel10, Channel11,
        Channel12, Channel13, Channel14, Channel15,
        TempSensor=16, VrefInt=17, Vbat=18
    } Channel;
    
    typedef enum {ModeSingle, ModeDual, ModeTriple} Mode;
    typedef enum {Res6bit, Res8bit, Res10bit, Res12bit, Res16bit} Resolution;
    
    typedef enum {EdgeNone, EdgeRising, EdgeFalling, EdgeBoth} Edge;
    typedef enum
    {
        TriggerTim1CC1 = ADC_ExternalTrigConv_T1_CC1,
        TriggerTim1CC2 = ADC_ExternalTrigConv_T1_CC2,
        TriggerTim1CC3 = ADC_ExternalTrigConv_T1_CC3,
        TriggerTim2CC2 = ADC_ExternalTrigConv_T2_CC2,
        TriggerTim2CC3 = ADC_ExternalTrigConv_T2_CC3,
        TriggerTim2CC4 = ADC_ExternalTrigConv_T2_CC4,
        TriggerTim2 = ADC_ExternalTrigConv_T2_TRGO,
        TriggerTim3CC1 = ADC_ExternalTrigConv_T3_CC1,
        TriggerTim3 = ADC_ExternalTrigConv_T3_TRGO,
        TriggerTim4CC4 = ADC_ExternalTrigConv_T4_CC4,
        TriggerTim5CC1 = ADC_ExternalTrigConv_T5_CC1,
        TriggerTim5CC2 = ADC_ExternalTrigConv_T5_CC2,
        TriggerTim5CC3 = ADC_ExternalTrigConv_T5_CC3,
        TriggerTim8CC1 = ADC_ExternalTrigConv_T8_CC1,
        TriggerTim8 = ADC_ExternalTrigConv_T8_TRGO,
        TriggerExtIT11 = ADC_ExternalTrigConv_Ext_IT11
    } Trigger;
    
    typedef enum
    {
        SampleTime_3Cycles = ADC_SampleTime_3Cycles,
        SampleTime_15Cycles = ADC_SampleTime_15Cycles,
        SampleTime_28Cycles = ADC_SampleTime_28Cycles,
        SampleTime_56Cycles = ADC_SampleTime_56Cycles,
        SampleTime_84Cycles = ADC_SampleTime_84Cycles,
        SampleTime_112Cycles = ADC_SampleTime_112Cycles,
        SampleTime_144Cycles = ADC_SampleTime_144Cycles,
        SampleTime_480Cycles = ADC_SampleTime_480Cycles,
    } SampleTime;
  
private:
    ADC_TypeDef *mAdc;//, *mAdc2, *mAdc3;
    ADC_InitTypeDef mConfig;
    Dma *mDma;
    bool mDmaOwner;
    ByteArray mBuffer;
    unsigned char mChannelResultMap[19];
  
    Mode mMode;
    bool mEnabled;
    Resolution mResolution;
    int mChannelCount;
    
public:
    Adc(int adcBase = 1); // for single mode
//    Adc(int adcBase1, int adcBase2); // for dual mode
//    Adc(int adcBase1, int adcBase2, int adcBase3); // for triple mode
    ~Adc();
    
    void setResolution(Resolution resolution);
    void selectTrigger(Trigger trigger, Edge edge);
    
    void addChannel(int channel, SampleTime sampleTime = SampleTime_3Cycles);
    
    void setEnabled(bool enable);
    bool isEnabled() const {return mEnabled;}
    void start() {setEnabled(true);}
    void stop() {setEnabled(false);}
    
    void startConversion();
    void setContinuousMode(bool enabled);
    
    void configDma(Dma *dma);
    
    int result(Channel channel);
};

#endif