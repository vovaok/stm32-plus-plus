#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <list>
#include <string>
#include <stdio.h>
#include "rcc.h"
#include "cpuid.h"
#include "core/core.h"
#include "stm32_conf.h"
//---------------------------------------------------------------------------

using namespace std;
//---------------------------------------------------------------------------

#if !defined(SYSTEM_CLOCK_MS)
#define SYSTEM_CLOCK_MS         1
#endif

#ifndef APP_NAME
#define APP_NAME        "Project"
#endif

#ifndef APP_DESCRIPTION
#define APP_DESCRIPTION ""
#endif

#ifndef APP_COMPANY
#define APP_COMPANY     "Neurobotics"
#endif

#if !defined(APP_VERSION)
#define APP_VERSION 0x0100
#endif

#if !defined(APP_BURNCOUNT)
#define APP_BURNCOUNT 0
#endif

#if !defined(APP_CLASS)
#define APP_CLASS 0xFFFF0000
#endif

typedef struct
{
    char pre[12];
    unsigned long cid;
    unsigned short ver;
    unsigned short pageSize;
    unsigned long length;
    unsigned long checksum;
    char timestamp[25];
} __appinfo_t__;
#define APP_DECLARE_BOOT_INFO() __root static const __appinfo_t__ __appinfo__ = {"__APPINFO__", APP_CLASS, APP_VERSION, 0, 0xDeadFace, 0xBaadFeed, __TIMESTAMP__}
//---------------------------------------------------------------------------

extern "C" void SysTick_Handler();
extern "C" void HardFault_Handler();

#pragma pack(4)
class Application
{
private:
    static Application* self;
    static const int mSysClkPeriod = SYSTEM_CLOCK_MS;
    
    std::list<TaskEvent> mTaskEvents;
    std::list<TickEvent> mTickEvents;
    typedef std::list<TaskEvent>::iterator TaskIterator;
    typedef std::list<TickEvent>::iterator TickIterator;
    bool m_tasksModified;
    
    static unsigned short mVersion;
    static string mBuildDate;
    static string mCpuInfo;
    static unsigned long mBurnCount;  
    static string mName;
    static string mDescription;
    static string mCompany;
    
    unsigned long mTimestamp;
    
    static void sysTickHandler();
    
    friend void SystemInit();
    friend void SysTick_Handler();
    friend void HardFault_Handler();

protected:
    Application()
    {
        self = this;
        mName = APP_NAME;
        mDescription = APP_DESCRIPTION;
        mCompany = APP_COMPANY;
        mVersion = APP_VERSION;
        mBurnCount = APP_BURNCOUNT;
        mBuildDate = string(__DATE__ " " __TIME__);
        char tempstr[64];
        sprintf(tempstr, "%s @ %d MHz, %dK flash", CpuId::name(), (int)(Rcc::sysClk() / 1000000), CpuId::flashSizeK());
        mCpuInfo = string(tempstr);
    }
    void setVersion(unsigned short ver) {mVersion = ver;}
    void setBurnCount(unsigned long cnt) {mBurnCount = cnt;}
    
public:
    /*! Экземпляр приложения. */
    static Application *instance() {return self;}

    /*! Запуск петли задач. */
    void exec();
    
    int registerTaskEvent(TaskEvent event);
    void unregisterTaskEvent(int id);
    int registerTickEvent(TickEvent event);
    void unregisterTickEvent(int id);    
    
    static string name() {return Application::mName;}
    static string description() {return Application::mDescription;}
    static string company() {return Application::mCompany;}
    
    /*! Версия приложения.
        Возвращает версию приложения, указанную в 'config.h'.
        Формат версии - число, 4 байта. Например: v.2.3.42.137 выглядит как 0x02032A89
    */
    static unsigned long version() {return Application::mVersion;}
    
    /*! Дата и время сборки.
        Возвращет дату и время сборке в виде строки в формате, какой её предоставляет IAR
    */
    static string buildDate() {return Application::mBuildDate;}
    
    /*! Информация о процессоре.
        Возвращает строку, содержащую информацию о процессоре: тип, ядро, частота.
    */
    static string cpuInfo() {return Application::mCpuInfo;}
    
    /*! Бёрн каунт.
        Каждый раз когда попахивает ключами, бёрнКаунт++;
    */
    static unsigned long burnCount() {return Application::mBurnCount;}
    
    //static int tickPeriodMs() {return Application::mSysClkPeriod;}
    
    /*! Запуск обновления прошивки
        Переходит к выполнению программы обновления (если она имеется)
    */
    static bool startOnbBootloader();
    
    /*! Возвращает время в миллисекундах с момента запуска.    
    */
    inline unsigned long timestamp() const {return mTimestamp;}
};

/*! Экземпляр приложения.
    Функция эквивалентна вызову Application::instance();
*/
Application *stmApp();

#endif
