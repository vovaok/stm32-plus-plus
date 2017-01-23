#include "application.h"

Application *Application::self = 0L;

unsigned short Application::mVersion = 0x0000;
string Application::mBuildDate = __DATE__ " " __TIME__;
string Application::mCpuInfo = "unknown";
unsigned long Application::mBurnCount = 0;
//---------------------------------------------------------------------------

void Application::sysTickHandler()
{
    if (!Application::self)
        return;
    
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

bool Application::startOnbBootloader()
{
    unsigned long *ptr = (unsigned long*)0x08000000;
    if (*ptr == 0xFFFFFFFF)
        return false;
    void (*f)(void) = reinterpret_cast<void(*)(void)>(*(ptr + 1));
    __disable_irq();
    for (int i=0; i<3; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __set_MSP(*ptr);
    f();
    return true; // po idee ne doljno suda zahodit
}
//---------------------------------------------------------------------------

#ifdef __cplusplus
 extern "C" {
#endif 

void SysTick_Handler(void)
{  
    Application::sysTickHandler();
}

//void HardFault_Handler(void)
//{
//    SysTick->CTRL = 0;
//    Application::startOnbBootloader();
//}

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
