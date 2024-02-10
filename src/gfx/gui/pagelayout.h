#pragma once

#include "layout.h"

class PageLayout : public Layout
{
public:
    void setIndex(int index)
    {
        m_index = index;
        update();
    }
    
    int currentIndex() const {return m_index;}
    Widget *currentWidget() {return items().size()? items().at(m_index): nullptr;}

protected:
    virtual void update() override
    {
        int idx = 0;
        for (Widget *widget: items())
        {
            widget->setVisible(m_index == idx++);
            setWidgetGeometry(widget, 0, 0, //m_widget->x(), m_widget->y(),
                              m_widget->width(), m_widget->height());
        }
    }
    
private:
    int m_index = 0;
};
