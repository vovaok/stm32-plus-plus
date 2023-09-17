#pragma once

#include "widget.h"
#include "../image.h"
#include "core/coretypes.h"

class PushButton: public Widget
{
public:
    PushButton(const ByteArray &text="", Widget *parent=nullptr);
    
    void setText(const ByteArray &text);
    const ByteArray &text() const {return m_text;}
    
    NotifyEvent onClick;
    void click();
    
    bool isDown() const {return m_down;}
    
//    void update();
    
protected:
    void paintEvent(Display *d);
    void pressEvent(int x, int y);
    void releaseEvent(int x, int y);
    
private:
    ByteArray m_text;
    bool m_down = false;
    Color m_upColor;
    Color m_downColor;
    
    int textWidth() const;
    int textHeight() const;
};