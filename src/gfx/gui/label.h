#pragma once

#include "../image.h"
#include "widget.h"

class Label : public Widget
{
public:
    Label(Widget *parent = nullptr);
    Label(const ByteArray &text, Widget *parent = nullptr);
    
    void setAlignment(Alignment value);
    Alignment alignment() const {return m_align;}
    
    const ByteArray &text() const {return m_text;}
    void setText(const ByteArray &text);
    void setNum(int n);
    void setNum(float n, int prec=-1);
    
    void update();
   
protected:
    void paintEvent(Display *d);
    
private:
    ByteArray m_text;
    Alignment m_align = AlignLeft;
    
    int textWidth() const;
    int textHeight() const;
};