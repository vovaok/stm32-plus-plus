#pragma once

#include "widget.h"
#include "../image.h"

class PushButton: public Widget
{
public:
    PushButton(const ByteArray &text="", Widget *parent=nullptr);
    
    void setText(const ByteArray &text);
    const ByteArray &text() const {return m_text;}

//    void update();
    
protected:
    void paintEvent(Display *d);
    
private:
    ByteArray m_text;
    
    int textWidth() const;
    int textHeight() const;
};