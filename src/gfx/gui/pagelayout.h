#pragma once

#include "layout.h"

class PageLayout : public Layout
{
public:
    void setPage(int index)
    {
        m_index = index;
        update();
    }

protected:
    virtual void update() override
    {
        int idx = 0;
        for (Widget *widget: items())
        {
            widget->setVisible(m_index == idx++);
            setWidgetGeometry(widget, m_widget->x(), m_widget->y(),
                              m_widget->width(), m_widget->height());
        }
    }
    
private:
    int m_index = 0;
};