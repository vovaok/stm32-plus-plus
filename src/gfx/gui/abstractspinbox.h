#pragma once

#include "widget.h"
#include "lineedit.h"

class AbstractSpinBox: public Widget
{
public:
    AbstractSpinBox(Widget *parent=nullptr);
    
    enum ButtonSymbols
    {
        Arrows,
        PlusMinus,
        NoButtons
    };
    
    void setButtonSymbols(ButtonSymbols bs);

    void stepUp();
    void stepDown();
    virtual void stepBy(int steps) = 0;
    virtual void reset() {}
    
    const ByteArray &text() {return m_edit->text();}
    
    void setBackgroundColor(Color c) {m_edit->setBackgroundColor(c);}
    
protected:
    virtual void updateText() = 0;
    LineEdit *lineEdit() {return m_edit;}
    const LineEdit *lineEdit() const {return m_edit;}
    
private:
    LineEdit *m_edit;
};
