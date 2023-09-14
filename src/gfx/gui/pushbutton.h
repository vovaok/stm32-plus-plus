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
    
    NotifyEvent onClick; // it is not implemented for now

//    void update();
    
protected:
    void paintEvent(Display *d);
    
private:
    ByteArray m_text;
    
    int textWidth() const;
    int textHeight() const;
};