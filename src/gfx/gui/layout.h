#pragma once

#include "widget.h"

class Layout
{
public:
//    Layout();
    
protected:    
    virtual void update() = 0;
    
    friend class Widget;
    Widget *m_widget = nullptr;
    bool m_needUpdate = true;
    
    std::vector<Widget *> items()
    {
        return m_widget->m_children;
    }
    
    void setWidgetGeometry(Widget *widget, int x, int y, int w, int h)
    {
        if (widget)
        {
            widget->m_x = x;
            widget->m_y = y;
            widget->m_width = w;
            widget->m_height = h;
        }
    }
    
private:
    

};