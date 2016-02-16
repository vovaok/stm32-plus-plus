#include "application.h"

#if !defined(SYSTEM_CLOCK_MS)
#define SYSTEM_CLOCK_MS         1
#endif
//---------------------------------------------------------------------------

Application *Application::self = 0L;

unsigned long Application::mVersion = 0x01000000;
string mBuildDate = __DATE__ " " __TIME__;
string mCpuInfo = "unknown";
unsigned long mBurnCount = 0;
//---------------------------------------------------------------------------

Application::Application()
{
    self = this;
}
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

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
