#ifndef _HARD_TIMER_H
#define _HARD_TIMER_H

#include "stm32_conf.h"
#include "core/core.h"
#include "gpio.h"

#define _us  
#define _ms     *1000
#define _s      *1000000
#define _Hz
#define _kHz    *1000
#define _MHz    *1000000

typedef enum
{
    TimNone = 0,
    Tim1    = 1,
    Tim2    = 2,
    Tim3    = 3,
    Tim4    = 4,
    Tim5    = 5,
    Tim6    = 6,
    Tim7    = 7,
    Tim8    = 8,
    Tim9    = 9,
    Tim10   = 10,
    Tim11   = 11,
    Tim12   = 12,
    Tim13   = 13,
    Tim14   = 14,
    Tim15   = 15,
    Tim16   = 16,
    Tim17   = 17,
    Tim18   = 18,
    Tim19   = 19,
} TimerNumber;

typedef enum
{
    ChNone = 0x00,
    Ch1 = 0x01,
    Ch2 = 0x02,
    Ch3 = 0x04,
    Ch4 = 0x08
} ChannelNumber;

class HardwareTimer
{
public:
    typedef enum
    {
        TrgReset    = TIM_TRGOSource_Reset,
        TrgEnable   = TIM_TRGOSource_Enable,
        TrgUpdate   = TIM_TRGOSource_Update,
        TrgOC1      = TIM_TRGOSource_OC1,
        TrgOC1Ref   = TIM_TRGOSource_OC1Ref,
        TrgOC2Ref   = TIM_TRGOSource_OC2Ref,
        TrgOC3Ref   = TIM_TRGOSource_OC3Ref,
        TrgOC4Ref   = TIM_TRGOSource_OC4Ref
    } TrgSource;
    
    typedef enum
    {
        TsITR0      = 0,
        TsITR1      = 1,
        TsITR2      = 2,
        TsITR3      = 3,
        TsTI1F_ED   = 4,
        TsTI1FP1    = 5,
        TsTI2FP2    = 6
    } InputTrigger;
    
    typedef enum
    {
        SmDisabled  = 0,
        SmEncoder1  = 1,
        SmEncoder2  = 2,
        SmEncoder3  = 3,
        SmReset     = 4,
        SmGated     = 5,
        SmTrigger   = 6,
        SmExtClock  = 7
    } SlaveMode;
    
    typedef enum
    {
        None,
        Rising,
        Falling,
        BothEdge
    } Polarity;
      
private:
    typedef enum
    {
        isrcUpdate  = 0,
        isrcCC1     = 1,
        isrcCC2     = 2,
        isrcCC3     = 3,
        isrcCC4     = 4,
        isrcCom     = 5,
        isrcTrigger = 6,
        isrcBreak   = 7
    } InterruptSource;
  
    TIM_TypeDef* mTim;  
    IRQn mIrq;
    NotifyEvent emitEvent[8];
    bool mEnabledIrq[8];
    
    unsigned long mInputClk;
    bool mEnabled;
    
    void enableInterrupt(InterruptSource source);
    
protected:
    TIM_TypeDef* tim() {return mTim;}
    const TIM_TypeDef* tim() const {return mTim;}
    
    unsigned long inputClk() const {return mInputClk;}
  
public:
    /*! Инициализация аппаратного таймера.
        \param timerNumber Номер аппаратного таймера.
        \param frequency_Hz Частота таймера в герцах. Для красоты можно использовать макросы _Hz, _kHz, _MHz.
                            Если задать частоту 0, то таймер будет проинициализирован без предделителя и с максимальным периодом.
    */
    HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz=0);
    
    static HardwareTimer* mTimers[14]; // should be private
    void handleInterrupt(); // should be private
    
    static TimerNumber getTimerByPin(Gpio::Config pinConfig);
    static ChannelNumber getChannelByPin(Gpio::Config pinConfig);
    
    void selectInputTrigger(InputTrigger trgi);
    void setSlaveMode(SlaveMode sms);
    void selectOutputTrigger(TrgSource source);
    void setFrequency(int frequency_Hz);
    
    void start();
    void stop();
    void setEnabled(bool enable);
    bool isEnabled() const {return mEnabled;}
    void setOnePulseMode(bool enabled);
    
    bool isReady() const;
    
    unsigned int counter() const {return mTim->CNT;}
    void setCounter(unsigned int value) {mTim->CNT = value;}
    unsigned int autoReloadRegister() const {return mTim->ARR;}
    void setAutoReloadRegister(unsigned int value) {mTim->ARR = value;}
    unsigned int prescaler() const {return mTim->PSC;}
    void setPrescaler(unsigned int value) {mTim->PSC = value;}
    unsigned int capture1() const {return mTim->CCR1;}
    unsigned int capture2() const {return mTim->CCR2;}
    unsigned int capture3() const {return mTim->CCR3;}
    unsigned int capture4() const {return mTim->CCR4;}
    void setCompare1(unsigned int value) {mTim->CCR1 = value;}
    void setCompare2(unsigned int value) {mTim->CCR2 = value;}
    void setCompare3(unsigned int value) {mTim->CCR3 = value;}
    void setCompare4(unsigned int value) {mTim->CCR4 = value;}
    void setCompareValue(ChannelNumber ch, unsigned int value);
    
    void setUpdateEvent(NotifyEvent event)   {emitEvent[isrcUpdate] = event; enableInterrupt(isrcUpdate);}
    void setCC1Event(NotifyEvent event)      {emitEvent[isrcCC1] = event; enableInterrupt(isrcCC1);}
    void setCC2Event(NotifyEvent event)      {emitEvent[isrcCC2] = event; enableInterrupt(isrcCC2);}
    void setCC3Event(NotifyEvent event)      {emitEvent[isrcCC3] = event; enableInterrupt(isrcCC3);}
    void setCC4Event(NotifyEvent event)      {emitEvent[isrcCC4] = event; enableInterrupt(isrcCC4);}
    void setComEvent(NotifyEvent event)      {emitEvent[isrcCom] = event; enableInterrupt(isrcCom);}
    void setTriggerEvent(NotifyEvent event)  {emitEvent[isrcTrigger] = event; enableInterrupt(isrcTrigger);}
    void setBreakEvent(NotifyEvent event)    {emitEvent[isrcBreak] = event; enableInterrupt(isrcBreak);}
    
    void setCaptureEvent(ChannelNumber ch, NotifyEvent event);
    void configCapture(ChannelNumber ch, Polarity pol);
    unsigned int captureValue(ChannelNumber ch) const;
    
    void configPWM(ChannelNumber ch, bool inverted=false);
    void setPWMEnabled(ChannelNumber ch, bool enabled);
};

#endif
