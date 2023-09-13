#pragma once

#include "core/application.h"
#include "core/timer.h"
#include "../display.h"
#include "widget.h"
#include "font_Tahoma_13.cpp"

class GuiApplication : public Application
{
public:
    GuiApplication();
    
    static Widget *widget();    
    static Display *display();
    
protected:
    void setDisplay(Display *d);
    Widget *rootWidget() {return m_widget;}
    
private:
    Widget *m_widget = nullptr; // root widget
    Display *m_display = nullptr;
    Timer *m_paintTimer;
    
    void paintTask();
};
