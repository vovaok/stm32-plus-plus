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
    char const pre[12];
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
    
    static unsigned short mVersion;
    static string mBuildDate;
    static string mCpuInfo;
    static unsigned long mBurnCount;  
    static string mName;
    static string mDescription;
    static string mCompany;
    
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
        mBuildDate = string(__DATE__" "__TIME__);
        char tempstr[64];
        sprintf(tempstr, "%s @ %d MHz, %dK flash", CpuId::name(), (int)(Rcc::sysClk() / 1000000), CpuId::flashSizeK());
        mCpuInfo = string(tempstr);
    }
    void setVersion(unsigned short ver) {mVersion = ver;}
    void setBurnCount(unsigned long cnt) {mBurnCount = cnt;}
    
public:
    /*! ��������� ����������. */
    static Application *instance() {return self;}

    /*! ������ ����� �����. */
    void exec();
    
    void registerTaskEvent(TaskEvent event);
    void unregisterTaskEvent(TaskEvent event);
    void registerTickEvent(TickEvent event);
    void unregisterTickEvent(TickEvent event);    
    
    static string name() {return Application::mName;}
    static string description() {return Application::mDescription;}
    static string company() {return Application::mCompany;}
    
    /*! ������ ����������.
        ���������� ������ ����������, ��������� � 'config.h'.
        ������ ������ - �����, 4 �����. ��������: v.2.3.42.137 �������� ��� 0x02032A89
    */
    static unsigned long version() {return Application::mVersion;}
    
    /*! ���� � ����� ������.
        ��������� ���� � ����� ������ � ���� ������ � �������, ����� � ������������� IAR
    */
    static string buildDate() {return Application::mBuildDate;}
    
    /*! ���������� � ����������.
        ���������� ������, ���������� ���������� � ����������: ���, ����, �������.
    */
    static string cpuInfo() {return Application::mCpuInfo;}
    
    /*! ���� �����.
        ������ ��� ����� ���������� �������, ��������++;
    */
    static unsigned long burnCount() {return Application::mBurnCount;}
    
    //static int tickPeriodMs() {return Application::mSysClkPeriod;}
    
    /*! ������ ���������� ��������
        ��������� � ���������� ��������� ���������� (���� ��� �������)
    */
    static bool startOnbBootloader();
};

/*! ��������� ����������.
    ������� ������������ ������ Application::instance();
*/
Application *stmApp();

#endif
