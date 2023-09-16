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
    
protected:
    virtual void stepBy(int steps) = 0;
    virtual void updateText() = 0;
    LineEdit *lineEdit() {return m_edit;}
    const LineEdit *lineEdit() const {return m_edit;}
    
private:
    LineEdit *m_edit;
};