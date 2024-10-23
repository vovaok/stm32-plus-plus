#ifndef _ADC303_H
#define _ADC303_H

#include "stm32.h"

#if defined(STM32G4)

#else

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

    typedef enum
    {
        Res6bit = ADC_CFGR_RES,
        Res8bit = ADC_CFGR_RES_1,
        Res10bit = ADC_CFGR_RES_0,
        Res12bit = 0,
        Res16bit = ADC_CFGR_ALIGN
    } Resolution;

    typedef enum
    {
        EdgeNone = 0,
        EdgeRising = ADC_CFGR_EXTEN_0,
        EdgeFalling = ADC_CFGR_EXTEN_1,
        EdgeBoth = ADC_CFGR_EXTEN_0 | ADC_CFGR_EXTEN_1
    } Edge;

    typedef enum
    {
        TriggerTim1CC1 = 0,
        TriggerTim1CC2 = ADC_CFGR_EXTSEL_0,
        TriggerTim1CC3 = ADC_CFGR_EXTSEL_1,
        TriggerTim1    = (ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_0),
        TriggerTim1_2  = (ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_1),
        TriggerTim2CC2 = (ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0),        
        TriggerTim2    = (ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0),
        TriggerTim3CC4 = ADC_CFGR_EXTSEL,
        TriggerTim3    = ADC_CFGR_EXTSEL_2,
        TriggerTim4CC4 = (ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_0),
        TriggerTim4    = (ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_2),
        TriggerTim6    = (ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_0),
        TriggerTim15   = (ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_1),
        TriggerTim8    = (ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0),
        TriggerExtIT11 = (ADC_CFGR_EXTSEL_2 | ADC_CFGR_EXTSEL_1)
    } Trigger;  
    typedef enum
    {
        SampleTime_1Cycles = 0,
        SampleTime_2Cycles = ADC_SMPR2_SMP10_0,
        SampleTime_4Cycles = ADC_SMPR2_SMP10_1,
        SampleTime_7Cycles = (ADC_SMPR2_SMP10_1 | ADC_SMPR2_SMP10_0),
        SampleTime_19Cycles = ADC_SMPR2_SMP10_2,
        SampleTime_61Cycles = (ADC_SMPR2_SMP10_2 | ADC_SMPR2_SMP10_0),
        SampleTime_181Cycles = (ADC_SMPR2_SMP10_2 | ADC_SMPR2_SMP10_1),
        SampleTime_601Cycles = ADC_SMPR2_SMP10,
        
    } SampleTime;    


private:
    static Adc *mInstances[3];
    ADC_TypeDef *mAdc;//, *mAdc2, *mAdc3;
//    ADC_InitTypeDef mConfig;
    Dma *mDma;
    bool mDmaOwner;
    Dma::Channel mDmaChannel;
    ByteArray mBuffer;
    unsigned char mChannelResultMap[19];

    Mode mMode;
    bool mEnabled;
    Resolution mResolution;
    int mChannelCount;
    int mSampleCount;

    NotifyEvent mCompleteEvent;

    void regularChannelConfig(Channel channel, uint8_t rank, SampleTime sampleTime);

public:
    Adc(int adcBase = 1); // for single mode
//    Adc(int adcBase1, int adcBase2); // for dual mode
//    Adc(int adcBase1, int adcBase2, int adcBase3); // for triple mode
    ~Adc();

    static Adc *instance(int periphNumber);

    void setResolution(Resolution resolution);
    void selectTrigger(Trigger trigger, Edge edge);

    void addChannel(Channel channel, SampleTime sampleTime = (SampleTime)0);
    Channel addChannel(Gpio::Config pin, SampleTime sampleTime = (SampleTime)0);
    void setMultisample(int sampleCount);

    int maxValue() const;
    
    Dma* getDma () {return mDma;}

    void setEnabled(bool enable);
    bool isEnabled() const {return mEnabled;}
    void start() {setEnabled(true);}
    void stop() {setEnabled(false);}

    void startConversion();
    bool isComplete() const;
    void setContinuousMode(bool enabled);

    void configDma(Dma *dma);

    int result(unsigned char channel);
    int resultByIndex(unsigned char index);
    float averageByIndex(uint8_t index);
    int lastResultByIndex(unsigned char index);
    const ByteArray &buffer() const {return mBuffer;}
    const uint16_t *samples() const {return reinterpret_cast<const uint16_t*>(mBuffer.data());}
    int sampleCount() const {return mBuffer.size() / 2;}
    const unsigned short &buffer(unsigned char channel) const;
    const unsigned short &bufferByIndex(unsigned char index) const;
    int currentSampleIndex() const {return sampleCount() - mDma->dataCounter();}

    void setCompleteEvent(NotifyEvent e);
};
#endif

#endif
