#pragma once

#include "widget.h"

class Layout
{
public:
//    Layout();
    virtual ~Layout() {}
  
    void setMargins(int value)
    {
        marginTop = marginRight = marginBottom = marginLeft = value;
        update();
    }
    
    void setMargins(int vertical, int horizontal)
    {
        marginTop = marginBottom = vertical;
        marginRight = marginLeft = horizontal;
        update();
    }
    
    void setMargins(int top, int right, int bottom, int left)
    {
        marginTop = top;
        marginRight = right;
        marginBottom = bottom;
        marginLeft = left;
        update();
    }
    
    void setMarginTop(int value)
    {
        marginTop = value;
        update();
    }
    
    void setMarginRight(int value)
    {
        marginRight = value;
        update();
    }
    
    void setMarginBottom(int value)
    {
        marginBottom = value;
        update();
    }
    
    void setMarginLeft(int value)
    {
        marginLeft = value;
        update();
    }
    
protected:    
    virtual void update() = 0;
    
    friend class Widget;
    Widget *m_widget = nullptr;
    bool m_needUpdate = true;
    int marginTop=0, marginRight=0, marginBottom=0, marginLeft=0;
    
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