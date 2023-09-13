#include "guiapplication.h"

GuiApplication::GuiApplication() : Application()
{
    FontDatabase::addApplicationFontFromData(font_Tahoma_13);
    
    m_widget = new Widget(nullptr);
    m_widget->setBackgroundColor(Color(192, 192, 192));
    m_paintTimer = new Timer;
    m_paintTimer->onTimeout = EVENT(&GuiApplication::paintTask);
    m_paintTimer->setInterval(16);
//        registerTaskEvent(EVENT(&GuiApplication::paintTask));
}

Widget *GuiApplication::widget()
{
    return static_cast<GuiApplication*>(instance())->rootWidget();
}

Display *GuiApplication::display()
{
    return static_cast<GuiApplication*>(instance())->m_display;
}

void GuiApplication::setDisplay(Display *d)
{
    m_display = d;
    m_widget->resize(d->width(), d->height());
    m_paintTimer->start();
}
    
void GuiApplication::paintTask()
{
    if (m_display)
    {
        m_widget->paint(m_display);
    }
}