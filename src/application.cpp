#include "application.h"

Application *Application::self = 0L;

unsigned short Application::mVersion = 0x0000;
string Application::mBuildDate = __DATE__ " " __TIME__;
string Application::mCpuInfo = "unknown";
unsigned long Application::mBurnCount = 0;
//---------------------------------------------------------------------------

void Application::sysTickHandler()
{
    Application *app = instance();
    std::list<TickEvent> &elist = app->mTickEvents;
    for (TickIterator it=elist.begin(); it!=elist.end(); it++)
    {
        (*it)(app->mSysClkPeriod);
    }
}

void Application::exec()
{       
    RCC_ClockSecuritySystemCmd(ENABLE);
    // configure system clock to mSysClkPeriod milliseconds
    if (SysTick_Config((Rcc::sysClk() / 1000) * mSysClkPeriod) != 0)
        throw Exception::badSoBad;
    
    // main loop
    while(1)
    {
        for (TaskIterator it=mTaskEvents.begin(); it!=mTaskEvents.end(); it++)
        {
            (*it)();
        }
        
        //__WFI(); // го слипать
    }
}
//---------------------------------------------------------------------------

void Application::registerTaskEvent(TaskEvent event)
{
    mTaskEvents.push_back(event);
}

void Application::unregisterTaskEvent(TaskEvent event)
{
    mTaskEvents.remove(event);
}

void Application::registerTickEvent(TickEvent event)
{
    mTickEvents.push_back(event);
}

void Application::unregisterTickEvent(TickEvent event)
{
    mTickEvents.remove(event);
}
//---------------------------------------------------------------------------

Application *stmApp()
{
    return Application::instance(); 
}
//---------------------------------------------------------------------------

#ifdef __cplusplus
 extern "C" {
#endif 

void SysTick_Handler(void)
{  
    Application::sysTickHandler();
}

void SystemInit(void) // on Reset_Handler
{
    /* FPU settings ------------------------------------------------------------*/
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    #endif
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;
    /* Reset CFGR register */
    RCC->CFGR = 0x00000000;
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010;
    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    /* Disable all interrupts */
    RCC->CIR = 0x00000000;

    Rcc::configPll(0, CpuId::maxSysClk());
//    Rcc::configPll(8000000, 168000000);
//    Rcc::configPll(16000000, 168000000);
//    Rcc::configPll(25000000, 168000000);

    /* Configure the Vector Table location add offset address ------------------*/
    #ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE;// | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
    #else
    SCB->VTOR = FLASH_BASE;// | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
    #endif
}

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
