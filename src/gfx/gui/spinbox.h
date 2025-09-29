#pragma once

#include "abstractspinbox.h"
#include "lineedit.h"

class SpinBox: public AbstractSpinBox
{
public:
    SpinBox(Widget *parent=nullptr);
    SpinBox(int min, int max, int def=0, int step=1, Widget *parent=nullptr);
    SpinBox(int min, int max, int def, int step, const ByteArray &suffix, Widget *parent=nullptr);
    
    void setValue(int value);
    int value() const {return m_value;}
    
    void setRange(int min, int max);
    void setMinimum(int value);
    int minimum() const {return m_min;}
    void setMaximum(int value);
    int maximum() const {return m_min;}
    void setStep(int value);
    int step() const {return m_step;}
    void setDefaultValue(int value);
    int defaultValue() const {return m_defaultValue;}
    void setPrefix(const ByteArray &s);
    const ByteArray &prefix() const {return m_prefix;}
    void setSuffix(const ByteArray &s);
    const ByteArray &suffix() const {return m_suffix;}
    
    std::function<void(int)> onValueChanged;
    
    virtual void stepBy(int steps) override;
    virtual void reset() override;
    
protected:    
    virtual void updateText() override;
    
private:
    int m_value = 0;
    int m_min = -0x7FFFFFFF;
    int m_max = 0x7FFFFFFF;
    int m_step = 1;
    int m_defaultValue = 0;
    ByteArray m_prefix, m_suffix;
};
