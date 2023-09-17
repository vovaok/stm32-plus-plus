#pragma once

#include "core/application.h"
#include "core/timer.h"
#include "../display.h"
#include "../touchscreen.h"
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
    void addTouchScreen(TouchScreen *ts);
    Widget *rootWidget() {return m_widget;}
    bool paintDone() const {return m_paintDone;} // first time painted
    
private:
    Widget *m_widget = nullptr; // root widget
    Widget *m_touchedWidget = nullptr; // current widget to receive touch events
    Display *m_display = nullptr;
    Timer *m_paintTimer;
    bool m_paintDone = false;
    
    void paintTask();
    void touchEvent(TouchEvent *event);
};
