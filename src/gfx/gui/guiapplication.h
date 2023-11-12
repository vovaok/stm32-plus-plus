#pragma once

#include "core/application.h"
#include "core/timer.h"
#include "../display.h"
#include "../touchscreen.h"
#include "palette.h"
#include "widget.h"
#include "font_Tahoma_13.cpp"

class GuiApplication : public Application
{
public:
    GuiApplication();
    static GuiApplication *instance();

    static Widget *widget();
    static Widget *focusWidget();
    static Palette* palette();
    static Display *display();
    static Font font() {return instance()->m_widget->font();}
    static void setFont(const Font &font);

protected:
    void setDisplay(Display *d);
    void addTouchScreen(TouchScreen *ts);

    Widget *rootWidget() {return m_widget;}
    bool paintDone() const {return m_paintDone;} // first time painted
    void setGuiAutoRepaint(bool enable) {m_autoRepaint = enable;}

    static void setFocusWidget(Widget *w);
    friend class Widget;

private:
    Widget *m_widget = nullptr; // root widget
    Widget *m_touchedWidget = nullptr; // current widget to receive touch events
    Widget *m_focusWidget = nullptr;
    Display *m_display = nullptr;
    Palette *m_palette = nullptr;
    Timer *m_paintTimer;
    bool m_paintDone = false;
    bool m_autoRepaint = true;
    char m_decimalPoint = '.';

    void paintTask();
    void touchEvent(TouchEvent *event);
};
