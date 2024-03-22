#ifndef _HARDWARETIMER_H
#define _HARDWARETIMER_H

//#include "stm32_conf.h"
#include "core/core.h"
#include "gpio.h"
#include "rcc.h"

//#define _us  
//#define _ms     *1000
//#define _s      *1000000
//#define _Hz
//#define _kHz    *1000
//#define _MHz    *1000000

#if defined(STM32F4)
#define TIM1_UP_IRQ         TIM1_UP_TIM10_IRQ
#define TIM1_TRG_COM_IRQ    TIM1_TRG_COM_TIM11_IRQ
#define TIM1_BRK_IRQ        TIM1_BRK_TIM9_IRQ
#define TIM8_UP_IRQ         TIM8_UP_TIM13_IRQ
#define TIM8_TRG_COM_IRQ    TIM8_TRG_COM_TIM14_IRQ
#define TIM8_BRK_IRQ        TIM8_BRK_TIM12_IRQ
#define TIM9_IRQ            TIM1_BRK_TIM9_IRQ
#define TIM10_IRQ           TIM1_UP_TIM10_IRQ
#define TIM11_IRQ           TIM1_TRG_COM_TIM11_IRQ
#define TIM12_IRQ           TIM8_BRK_TIM12_IRQ
#define TIM13_IRQ           TIM8_UP_TIM13_IRQ
#define TIM14_IRQ           TIM8_TRG_COM_TIM14_IRQ

#elif defined(STM32L4) || defined(STM32G4)
#define TIM1_UP_IRQ         TIM1_UP_TIM16_IRQ
    #ifndef TIM1_TRG_COM_IRQ // if not already defined
    #define TIM1_TRG_COM_IRQ    TIM1_TRG_COM_TIM17_IRQ
    #endif
#define TIM1_BRK_IRQ        TIM1_BRK_TIM15_IRQ
#define TIM8_UP_IRQ         TIM8_UP_IRQ
#define TIM8_TRG_COM_IRQ    TIM8_TRG_COM_IRQ
#define TIM8_BRK_IRQ        TIM8_BRK_IRQ
#define TIM15_IRQ           TIM1_BRK_TIM15_IRQ
#define TIM16_IRQ           TIM1_UP_TIM16_IRQ
#define TIM17_IRQ           TIM1_TRG_COM_TIM17_IRQ

    #if defined(STM32G4)
    #define TIM7_IRQ            TIM7_DAC_IRQ
    #endif

#endif

#define TIM6_IRQ            TIM6_DAC_IRQ

#define TIM_IRQn(x)         GLUE(TIM##x##_IRQ, n)
#define TIM_IRQHandler(x)   GLUE(TIM##x##_IRQ, Handler)

#define DECLARE_TIM_IRQ_HANDLER(x) extern "C" void TIM_IRQHandler(x)();
#define DECLARE_TIM_FRIEND(x) friend void TIM_IRQHandler(x)();
#define FOREACH_TIM_IRQ(f) \
    f(1_BRK) f(1_UP) f(1_TRG_COM) f(1_CC) \
    f(2) f(3)  f(4)  f(5)  f(6)  f(7) \
    f(8_BRK) f(8_UP) f(8_TRG_COM) f(8_CC) \
    f(9) f(10) f(11) f(12) f(13) f(14)

FOREACH_TIM_IRQ(DECLARE_TIM_IRQ_HANDLER)

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
        Tim20   = 20
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
    int inputClk() const {return mInputClk;}
    
    static TimerNumber getTimerByPin(Gpio::Config pinConfig);
    static ChannelNumber getChannelByPin(Gpio::Config pinConfig);
    
    void selectInputTrigger(InputTrigger trgi);
    void setSlaveMode(SlaveMode sms);
    void selectOutputTrigger(TrgSource source);
    void setFrequency(int frequency_Hz);
    int frequency() const; // current programmed frequency
    int clockFrequency() const; // current clock frequency
    
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
    void setChannelInverted(ChannelNumber ch, bool inverted);
    void setComplementaryChannelEnabled(ChannelNumber ch, bool enabled);
    void setComplementaryChannelInverted(ChannelNumber ch, bool inverted);
    
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
    
    FOREACH_TIM_IRQ(DECLARE_TIM_FRIEND)
};

#endif
