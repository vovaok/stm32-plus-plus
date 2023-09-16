#pragma once

#include "abstractspinbox.h"
#include "lineedit.h"

class FloatSpinBox: public AbstractSpinBox
{
public:
    FloatSpinBox(Widget *parent=nullptr);
    
    void setValue(float value);
    float value() const {return m_value;}
    
    void setRange(float min, float max);
    void setMinimum(float value);
    float minimum() const {return m_min;}
    void setMaximum(float value);
    float maximum() const {return m_min;}
    void setStep(float value);
    float step() const {return m_step;}
    void setPrefix(const ByteArray &s);
    const ByteArray &prefix() const {return m_prefix;}
    void setSuffix(const ByteArray &s);
    const ByteArray &suffix() const {return m_suffix;}
    void setPrecision(int digits);
    int precision() const {return m_precision;}

protected:    
    virtual void updateText();
    virtual void stepBy(int steps);
    
private:
    float m_value = 0;
    float m_min = 0;
    float m_max = 100;
    float m_step = 1;
    int m_precision = 3;
    ByteArray m_prefix, m_suffix;
};