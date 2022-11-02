#ifndef _HARDWARETIMER_H
#define _HARDWARETIMER_H

//#include "stm32_conf.h"
#include "core/core.h"
#include "gpio.h"
#include "rcc.h"

#define _us  
#define _ms     *1000
#define _s      *1000000
#define _Hz
#define _kHz    *1000
#define _MHz    *1000000

extern "C" void TIM1_BRK_TIM9_IRQHandler();
extern "C" void TIM1_UP_TIM10_IRQHandler();
extern "C" void TIM1_TRG_COM_TIM11_IRQHandler();
extern "C" void TIM1_CC_IRQHandler();
extern "C" void TIM2_IRQHandler();
extern "C" void TIM3_IRQHandler();
extern "C" void TIM4_IRQHandler();
extern "C" void TIM5_IRQHandler();
extern "C" void TIM6_DAC_IRQHandler();
extern "C" void TIM7_IRQHandler();
extern "C" void TIM8_BRK_TIM12_IRQHandler();
extern "C" void TIM8_UP_TIM13_IRQHandler();
extern "C" void TIM8_TRG_COM_TIM14_IRQHandler();
extern "C" void TIM8_CC_IRQHandler();
extern "C" void TIM9_IRQHandler();
extern "C" void TIM10_IRQHandler();
extern "C" void TIM11_IRQHandler();
extern "C" void TIM12_IRQHandler();
extern "C" void TIM13_IRQHandler();
extern "C" void TIM14_IRQHandler();
extern "C" void TIM15_IRQHandler();

class HardwareTimer
{
public:
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
        Ch1 = 0x0001,
        Ch2 = 0x0010,
        Ch3 = 0x0100,
        Ch4 = 0x1000
    } ChannelNumber;
  
    typedef enum
    {
        TrgReset    = 0x0000,
        TrgEnable   = 0x0010,
        TrgUpdate   = 0x0020,
        TrgOC1      = 0x0030,
        TrgOC1Ref   = 0x0040,
        TrgOC2Ref   = 0x0050,
        TrgOC3Ref   = 0x0060,
        TrgOC4Ref   = 0x0070
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
        Rising = 0x0,
        Falling = 0x2,
        BothEdge = 0xA
    } Polarity;
    
    /*! Инициализация аппаратного таймера.
    \param timerNumber Номер аппаратного таймера.
    \param frequency_Hz Частота таймера в герцах. Для красоты можно использовать макросы _Hz, _kHz, _MHz.
                        Если задать частоту 0, то таймер будет проинициализирован без предделителя и с максимальным периодом.
    */
    HardwareTimer(TimerNumber timerNumber, unsigned int frequency_Hz=0);
    
    TIM_TypeDef* tim() {return mTim;}
    const TIM_TypeDef* tim() const {return mTim;}
    unsigned long inputClk() const {return mInputClk;}
    
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
    
    bool dir() const {return mTim->CR1 & TIM_CR1_DIR;}
    unsigned int counter() const {return mTim->CNT;}
    void setCounter(unsigned int value) {mTim->CNT = value;}
    unsigned int repetitionCounter() const {return mTim->RCR;}
    void setRepetitionCounter(unsigned int value) {mTim->RCR = value;}
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
    void configCapture(ChannelNumber ch, Polarity polarity = Rising);
    void configCapture(Gpio::Config pin, Polarity polarity = Rising, NotifyEvent event=NotifyEvent());
    unsigned int captureValue(ChannelNumber ch) const;
    
    void setChannelEnabled(ChannelNumber ch, bool enabled);
    void setComplementaryChannelEnabled(ChannelNumber ch, bool enabled);
    
    inline void generateUpdateEvent() {mTim->EGR = TIM_EGR_UG;}
    inline void generateComEvent() {mTim->EGR = TIM_EGR_COMG;}
    
protected:
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
    
    typedef enum
    {
        PwmMode_Timing   = 0x0000,
        PwmMode_Active   = 0x0010,
        PwmMode_Inactive = 0x0020,
        PwmMode_Toggle   = 0x0030,
        PwmMode_PWM1     = 0x0060,
        PwmMode_PWM2     = 0x0070
    } PwmMode;
    
//    unsigned long inputClk() const {return mInputClk;}
    
    void configPwm(ChannelNumber ch, PwmMode pwmMode=PwmMode_PWM2, bool inverted=false);
      
private:  
    TIM_TypeDef* mTim;  
    IRQn_Type mIrq;
    NotifyEvent emitEvent[8];
    bool mEnabledIrq[8];
    
    unsigned long mInputClk;
    bool mEnabled;
    
    static HardwareTimer* mTimers[19];
    
    void enableInterrupt(InterruptSource source);
    void handleInterrupt();
    
    friend void TIM1_BRK_TIM9_IRQHandler();
    friend void TIM1_UP_TIM10_IRQHandler();
    friend void TIM1_TRG_COM_TIM11_IRQHandler();
    friend void TIM1_CC_IRQHandler();
    friend void TIM2_IRQHandler();
    friend void TIM3_IRQHandler();
    friend void TIM4_IRQHandler();
    friend void TIM5_IRQHandler();
    friend void TIM6_DAC_IRQHandler();
    friend void TIM7_IRQHandler();
    friend void TIM8_BRK_TIM12_IRQHandler();
    friend void TIM8_UP_TIM13_IRQHandler();
    friend void TIM8_TRG_COM_TIM14_IRQHandler();
    friend void TIM8_CC_IRQHandler();
    friend void TIM9_IRQHandler();
    friend void TIM10_IRQHandler();
    friend void TIM11_IRQHandler();
    friend void TIM12_IRQHandler();
    friend void TIM13_IRQHandler();
    friend void TIM14_IRQHandler();
    friend void TIM15_IRQHandler();
};

#endif
