#include "compf3.h"

#if defined(STM32F3)

Comp::Comp(int no) :
    mDev(0L)    
{
  
    if (no == 0 || no > 6)
        THROW(Exception::InvalidPeriph);  

    switch (no)
    {
#if defined (COMP1)
      case 1:
        mDev = COMP1;        
        break;
#endif
#if defined (COMP2)        
      case 2:
        mDev = COMP2;      
        break;
#endif
#if defined (COMP3)
      case 3:
        mDev = COMP3;        
        break;
#endif
#if defined (COMP4)
      case 4:
        mDev = COMP4;      
        break;
#endif
#if defined (COMP5)
      case 5:
        mDev = COMP5;        
        break;
#endif
#if defined (COMP6)        
      case 6:
        mDev = COMP6;      
        break;
#endif
    }
    
    if (!mDev)
        THROW(Exception::InvalidPeriph);    
  
    rcc().setPeriphEnabled(SYSCFG);
   // RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

}
//---------------------------------------------------------------------------

void Comp::setConfig(Config cfg)
{
    bool en = mConfig.COMPEN;
    cfg.COMPEN = 0;
    mConfig = cfg;
    
    mDev->CSR = mConfig.csr;   
    if (en)
    {
        mConfig.COMPEN = 1;
        mDev->CSR = mConfig.csr;   
    }
}

void Comp::updateConfig()
{
    mDev->CSR = mConfig.csr;   
}

//---------------------------------------------------------------------------
void Comp::setInvertedOut(bool invert)
{
  mConfig.COMPPOL = invert? 1 : 0;
  updateConfig();
  
}

void Comp::setInMSel(InMSelCode code)
{
  if(code>PA2_p)
  {
    mConfig.COMPINMSEL = 0;
    mConfig.COMPINMSEL_H = 1;
  }
  else
    mConfig.COMPINMSEL = code;
  updateConfig();
}

void Comp::setOutSel(OutSelCode code)
{
    mConfig.COMPOUTSEL = code;
    updateConfig();
    
}

void Comp::setEnable(bool en)
{
   
        mConfig.COMPEN = en? 1 : 0;
        mDev->CSR = mConfig.csr;   
    
}

#endif
