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
    void setDirection(Direction direction);

    void stepUp();
    void stepDown();
    virtual void stepBy(int steps) = 0;
    virtual void reset() {}
    
    const ByteArray &text() {return m_edit->text();}
    
    void setBackgroundColor(Color c) {m_edit->setBackgroundColor(c);}

    void setWrapping(bool wrapping);
    bool wrapping() const {return m_wrapping;}
    
protected:
    virtual void updateText() = 0;
    LineEdit *lineEdit() {return m_edit;}
    const LineEdit *lineEdit() const {return m_edit;}
    
private:
    LineEdit *m_edit;
    bool m_wrapping = false;
};
