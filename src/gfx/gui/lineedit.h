#pragma once

#include "../image.h"
#include "widget.h"

class LineEdit : public Widget
{
public:
    LineEdit(Widget *parent = nullptr);
    LineEdit(const ByteArray &text, Widget *parent = nullptr);
    
    void setAlignment(Alignment value);
    Alignment alignment() const {return m_align;}
    
    const ByteArray &text() const {return m_text;}
    void setText(const ByteArray &text);
    
    void update();
   
protected:
    void paintEvent(Display *d);
    
private:
    ByteArray m_text;
    Alignment m_align = AlignLeft;
    
    int textWidth() const;
    int textHeight() const;
};