#pragma once

#include "../image.h"
#include "widget.h"

class LedWidget : public Widget
{
public:
    LedWidget(Color color=Green, int size=15, Widget *parent=nullptr);
    
    void setSize(int value);
    
    void update();
    
    void setColor(Color color);
    Color color() const {return m_color;}
    void setState(bool on);
    bool state() const {return m_state;}
    void toggle();
    
protected:
    void paintEvent(Display *d);
    
private:
    Color m_color;
    Image m_img;
    bool m_state = false;
};