#if defined(STM32F37X)

#include "sdadc.h"

SDAdc::SDAdc(Gpio::Config regularChannelP, Gpio::Config regularChannelM)
{
    mDev = getDevByPin(regularChannelP);
    if (!mDev)
        throw Exception::invalidPeriph;
    
    if (regularChannelM != Gpio::NoConfig)
    {
        if (mDev != getDevByPin(regularChannelM))
            throw Exception::invalidPin;
    }
    
    if (GpioConfigGetPeriphChannel(regularChannelP) != GpioConfigGetPeriphChannel(regularChannelP))
        throw Exception::invalidPin;
    
    // PWR APB1 interface clock enable
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    
    if (mDev == SDADC1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_SDADC1EN;
        // SDADC analog pwr enable
        PWR->CR |= PWR_CR_SDADC1EN;
    }
    else if (mDev == SDADC2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_SDADC2EN;
        // SDADC analog pwr enable
        PWR->CR |= PWR_CR_SDADC2EN;
    }
    else if (mDev == SDADC3)
    {
        RCC->APB2ENR |= RCC_APB2ENR_SDADC3EN;
        // SDADC analog pwr enable
        PWR->CR |= PWR_CR_SDADC3EN;
    }
    
    /* Set the SDADC divider: The SDADC should run @6MHz */
    /* If Sysclk is 72MHz, SDADC divider should be 12 */
    RCC_SDADCCLKConfig(RCC_SDADCCLK_SYSCLK_Div48);
    
    Gpio::config(regularChannelP);
    Gpio::config(regularChannelM);
    
    if (regularChannelM == Gpio::NoConfig) // single ended mode
    {
#warning dopisat single-ended mode
    }
    
    // regular channel selection:
    mDev->CR2 &= ~SDADC_CR2_RCH;
    mDev->CR2 |= (GpioConfigGetPeriphChannel(regularChannelP) << 16) & SDADC_CR2_RCH;
    
//    // enable fast conversion by default:
//    mDev->CR2 |= SDADC_CR2_FAST;
}
//---------------------------------------------------------------------------

SDADC_TypeDef *SDAdc::getDevByPin(Gpio::Config c)
{
    switch (GpioConfigGetPeriphNumber(c))
    {
        case 0x1: return SDADC1;
        case 0x2: return SDADC2;
        case 0x3: return SDADC3;
        default: return 0L;
    }
}
//---------------------------------------------------------------------------

void SDAdc::setReference(Reference ref)
{
    SDADC1->CR1 &= ~SDADC_CR1_REFV;
    SDADC1->CR1 |= ref & SDADC_CR1_REFV;
}

void SDAdc::setPowerMode(PowerMode pmode)
{
    mDev->CR1 &= ~(SDADC_CR1_SLOWCK | SDADC_CR1_SBI | SDADC_CR1_PDI);
    mDev->CR1 |= pmode & (SDADC_CR1_SLOWCK | SDADC_CR1_SBI | SDADC_CR1_PDI);
}
//---------------------------------------------------------------------------

void SDAdc::setEnabled(bool enabled)
{
    if (enabled)
        mDev->CR2 |= SDADC_CR2_ADON;
    else
        mDev->CR2 &= ~SDADC_CR2_ADON;
}
//---------------------------------------------------------------------------

void SDAdc::startConversion()
{
    //if (!mDev->JCHGR) // if there aren't injected channels
    {
        // start regular conversion:
        mDev->CR2 |= SDADC_CR2_RSWSTART;
    }
}
    
void SDAdc::startContinuousMode()
{
   // if (!mDev->JCHGR) // if there aren't injected channels
    {
        // enable continuous mode for regular channel:
        mDev->CR2 |= SDADC_CR2_RCONT;
        // start regular conversion:
        mDev->CR2 |= SDADC_CR2_RSWSTART;
    }
}

void SDAdc::stopContinuousMode()
{
  //  if (!mDev->JCHGR) // if there aren't injected channels
    {
        // disable continuous mode for regular channel:
        mDev->CR2 &= ~SDADC_CR2_RCONT;
    }
}
//---------------------------------------------------------------------------

#endif