#ifndef _SDADC_H
#define _SDADC_H
#if defined(STM32F37X)

#include "stm32_conf.h"
#include "core/coreexception.h"
#include "gpio.h"
#include "rcc.h"

class SDAdc
{
public:
    typedef enum
    {
        RefExternal = 0 << 8,
        Ref1V2      = 1 << 8,
        Ref1V8      = 2 << 8,
        RefVdd      = 3 << 8
    } Reference;
    
    typedef enum
    {
        PowerMode_Normal            = 0,
        PowerMode_Slow              = (1<<10),
        PowerMode_StandbyWhenIdle   = (1<<11),
        PowerMode_PowerDownWhenIdle = (1<<12)
    } PowerMode;
  
private:
    SDADC_TypeDef *mDev;
    
    static SDADC_TypeDef *getDevByPin(Gpio::Config c);
  
public:
    SDAdc(Gpio::Config regularChannelP, Gpio::Config regularChannelM=Gpio::NoConfig);
    
    static void setReference(Reference ref);
    void setPowerMode(PowerMode pmode);
    
    void setEnabled(bool enabled = true);
    inline bool isEnabled() const {return mDev->CR2 & SDADC_CR2_ADON;}

//    void startCalibration();
    
    void startConversion();
    
    void startContinuousMode();
    void stopContinuousMode();
    
    inline bool isReady() const {return mDev->ISR & SDADC_ISR_REOCF;} // only regular channel supported!!
    inline short value() const {return mDev->RDATAR;}
};

#endif
#endif