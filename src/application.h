#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <list>
#include "rcc.h"
#include "core/core.h"
#include "stm32_conf.h"

using namespace std;

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
    
    static unsigned long mVersion;
    static string mBuildDate;
    static string mCpuInfo;
    static unsigned long mBurnCount;

protected:
    Application();
    
public:
    /*! ��������� ���������� (���������). */
    static Application *instance() {if (!self) self = new Application(); return self;}
    /*! ������ ����� �����. */
    void exec();
    
    void registerTaskEvent(TaskEvent event);
    void unregisterTaskEvent(TaskEvent event);
    void registerTickEvent(TickEvent event);
    void unregisterTickEvent(TickEvent event);
    
    /*! ���������� ���������� ���������� �������.
        ��� ������� ��� ����������� �������������, �������� �.
    */
    static void sysTickHandler();
    
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
};

/*! ��������� ����������.
    ������� ������������ ������ Application::instance();
*/
Application *stmApp();

#endif