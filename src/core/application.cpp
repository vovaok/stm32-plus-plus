#include "application.h"

Application *Application::self = 0L;
bool Application::m_tickFlag = false;

const char *Application::mName = APP_NAME;
const char *Application::mDescription = APP_DESCRIPTION;
const char *Application::mCompany = APP_COMPANY;
uint16_t Application::mVersion = 0x0000;
uint32_t Application::mBurnCount = 0;
const char * Application::mBuildDate = __DATE__ " " __TIME__;
string Application::mCpuInfo = "unknown";
//---------------------------------------------------------------------------

void Application::sysTickHandler()
{
    if (!Application::self)
        return;

    Application *app = instance();
    int dt = app->mSysClkPeriod;
    app->mTimestamp += dt;
    app->m_tickFlag = true;

#if __cplusplus > 199711L
    for (TickEvent &e: app->mTickEvents)
        e(dt);
#else
    std::list<TickEvent> &elist = app->mTickEvents;
    for (TickIterator it=elist.begin(); it!=elist.end(); it++)
    {
        (*it)(app->mSysClkPeriod);
    }
#endif
}

void Application::delay(int ms)
{
    Application *app = instance();
    uint32_t alarm = app->mTimestamp + ms;
    while (app->mTimestamp < alarm);
}

void Application::exec()
{
    //RCC_ClockSecuritySystemCmd(ENABLE);
    // configure system clock to mSysClkPeriod milliseconds
    if (SysTick_Config((rcc().sysClk() / 1000) * mSysClkPeriod) != 0)
        THROW(Exception::BadSoBad);

//    NVIC_SetPriorityGrouping(4);

    __enable_interrupt();

    // main loop
    while(1)
    {
        // update watchdog
//        if (m_watchdogEnabled) // dummy write to the register if it is not initialized
            IWDG->KR = 0xAAAA;
        
#if __cplusplus > 199711L
        for (TaskEvent &e: mTaskEvents)
        {
            if (m_tasksModified)
                break;
            e();
        }
#else
        for (TaskIterator it=mTaskEvents.begin(); it!=mTaskEvents.end(); it++)
        {
            if (m_tasksModified)
                break;
            (*it)();
        }
#endif
        m_tasksModified = false;

        if (m_sleeping)
            __WFI(); // го слипать
    }
}
//---------------------------------------------------------------------------

int Application::registerTaskEvent(TaskEvent event)
{
    m_tasksModified = true;
    mTaskEvents.push_back(event);
    return mTaskEvents.size() - 1;
}

void Application::unregisterTaskEvent(int id)
{
    if (id >= 0 && id < mTaskEvents.size())
    {
        m_tasksModified = true;
        TaskIterator it = mTaskEvents.begin();
        std::advance(it, id);
        mTaskEvents.erase(it);
    }
}

int Application::registerTickEvent(TickEvent event)
{
    mTickEvents.push_back(event);
    return mTickEvents.size() - 1;
}

void Application::unregisterTickEvent(int id)
{
    if (id >= 0 && id < mTickEvents.size())
    {
        TickIterator it = mTickEvents.begin();
        std::advance(it, id);
        mTickEvents.erase(it);
    }
}
//---------------------------------------------------------------------------

Application *stmApp()
{
    return Application::instance();
}
//---------------------------------------------------------------------------

void Application::systemReset()
{
    NVIC_SystemReset();
}

void Application::enableWatchdog()
{
    IWDG->KR = 0x5555;
    IWDG->PR = 1; 
    IWDG->RLR = 0xfff;  
    IWDG->KR = 0xCCCC;
}

bool Application::startOnbBootloader()
{
    unsigned long *ptr = (unsigned long*)0x08000000;
    if (*ptr == 0xFFFFFFFF)
        return false;
    
    rcc().setPeriphEnabled(PWR);
    PWR->CR |= PWR_CR_DBP;
    RTC->BKP0R = 0xb00710ad; // bootload
    PWR->CR &= ~PWR_CR_DBP;
    
    NVIC_SystemReset();
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
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    #endif

#if defined(STM32F3)
    /* FPU settings ------------------------------------------------------------*/

    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;
    /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, SDADCPRE and MCOSEL[2:0] bits */
    RCC->CFGR &= (uint32_t)0x00FF0000;
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;
    /* Reset PREDIV1[3:0] bits */
    RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;
    /* Reset USARTSW[1:0], I2CSW and CECSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFF0F8C;
    /* Disable all interrupts */
    RCC->CIR = 0x00000000;
#elif defined(STM32F4)
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
#elif defined(STM32L4)
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif
  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set MSION bit */
  RCC->CR |= RCC_CR_MSION;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON , HSION, and PLLON bits */
  RCC->CR &= (uint32_t)0xEAF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x00001000;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIER = 0x00000000;
#endif

#if !defined(STM32F0)
    /* Configure the Vector Table location add offset address ------------------*/
    #ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE;// | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
    #else
    SCB->VTOR = FLASH_BASE;// | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
    #endif

#else
   // 2. Настройка источника тактового сигнала (HSI - внутренний генератор 8 МГц)
    RCC->CR |= RCC_CR_HSION;            // Включаем HSI
    while (!(RCC->CR & RCC_CR_HSIRDY)); // Ждем, пока HSI стабилизируется

    // 3. Настройка системного тактового сигнала (SYSCLK)
    RCC->CFGR &= ~RCC_CFGR_SW;          // Очищаем биты выбора источника SYSCLK
    RCC->CFGR |= RCC_CFGR_SW_HSI;       // Выбираем HSI как источник SYSCLK
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); // Ждем переключения

    // 4. Настройка делителей для шин
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE); // Сбрасываем делители
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;    // HCLK = SYSCLK (без деления)
    RCC->CFGR |= RCC_CFGR_PPRE_DIV1;    // PCLK = HCLK (без деления)    

    
#endif
    
   
}

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
