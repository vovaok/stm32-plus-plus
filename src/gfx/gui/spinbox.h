#pragma once

#include "abstractspinbox.h"
#include "lineedit.h"

class SpinBox: public AbstractSpinBox
{
public:
    SpinBox(Widget *parent=nullptr);
    
    void setValue(int value);
    int value() const {return m_value;}
    
    void setRange(int min, int max);
    void setMinimum(int value);
    int minimum() const {return m_min;}
    void setMaximum(int value);
    int maximum() const {return m_min;}
    void setStep(int value);
    int step() const {return m_step;}
    void setPrefix(const ByteArray &s);
    const ByteArray &prefix() const {return m_prefix;}
    void setSuffix(const ByteArray &s);
    const ByteArray &suffix() const {return m_suffix;}
    
    Closure<void(int)> onValueChanged;
    
protected:    
    virtual void updateText();
    virtual void stepBy(int steps);
    
private:
    int m_value = 0;
    int m_min = -0x7FFFFFFF;
    int m_max = 0x7FFFFFFF;
    int m_step = 1;
    ByteArray m_prefix, m_suffix;
};