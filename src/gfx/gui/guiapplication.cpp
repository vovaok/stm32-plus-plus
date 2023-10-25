#include "guiapplication.h"

GuiApplication::GuiApplication() : Application()
{
    FontDatabase::addApplicationFontFromData(font_Tahoma_13);

    m_palette = new Palette();

    m_widget = new Widget(nullptr);
    m_widget->setBackgroundColor(Color(192, 192, 192));
    m_paintTimer = new Timer;
    m_paintTimer->onTimeout = EVENT(&GuiApplication::paintTask);
    m_paintTimer->setInterval(16);
//        registerTaskEvent(EVENT(&GuiApplication::paintTask));
}

GuiApplication *GuiApplication::instance()
{
  return static_cast<GuiApplication*>(Application::instance());
}

Widget *GuiApplication::widget()
{
    return instance()->rootWidget();
}

Widget *GuiApplication::focusWidget()
{
    return instance()->m_focusWidget;
}

Palette* GuiApplication::palette()
{
    return instance()->m_palette;
}

void GuiApplication::setFocusWidget(Widget *w)
{
    GuiApplication *a = instance();
    if (a->m_focusWidget)
        a->m_focusWidget->update();
    a->m_focusWidget = w;
}

Display *GuiApplication::display()
{
    return instance()->m_display;
}

void GuiApplication::setDisplay(Display *d)
{
    m_display = d;
    m_widget->resize(d->width(), d->height());
    m_paintTimer->start();
}

void GuiApplication::addTouchScreen(TouchScreen *ts)
{
    ts->onTouch = EVENT(&GuiApplication::touchEvent);
}

void GuiApplication::touchEvent(TouchEvent *event)
{
    if (m_widget)
    {
        if (!m_touchedWidget)
        {
            m_touchedWidget = m_widget->widgetAt(event->x(), event->y());
            while (!m_touchedWidget->m_acceptTouchEvents)
            {
                m_touchedWidget = m_touchedWidget->parent();
                if (!m_touchedWidget)
                    return;
            }
        }

        if (!m_touchedWidget->m_enabled)
        {
            m_touchedWidget = nullptr;
            return;
        }

        switch (event->type())
        {
        case TouchEvent::Press:
            m_touchedWidget->pressEvent(event->x(), event->y());
            break;

        case TouchEvent::Move:
            m_touchedWidget->moveEvent(event->x(), event->y());
            break;

        case TouchEvent::Release:
            m_touchedWidget->releaseEvent(event->x(), event->y());
            m_touchedWidget = nullptr;
            break;
        };
    }
}

void GuiApplication::paintTask()
{
    if (m_display && m_autoRepaint)
    {
        m_widget->paint(m_display);
        m_paintDone = true;
    }
}