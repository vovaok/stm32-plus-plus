#include "floatspinbox.h"
#include "core/macros.h"
#include "flexlayout.h"
#include "pushbutton.h"

FloatSpinBox::FloatSpinBox(Widget *parent) : AbstractSpinBox(parent)
{
    setValue(0);
    updateText();
}

FloatSpinBox::FloatSpinBox(float min, float max, float def, float step, int prec, Widget *parent) :
    AbstractSpinBox(parent),
    m_min(min),
    m_max(max),
    m_step(step),
    m_defaultValue(def),
    m_precision(prec)
{
    setValue(m_defaultValue);
    updateText();
}

FloatSpinBox::FloatSpinBox(float min, float max, float def, float step, int prec, const ByteArray &suffix, Widget *parent) :
    AbstractSpinBox(parent),
    m_min(min),
    m_max(max),
    m_step(step),
    m_defaultValue(def),
    m_precision(prec),
    m_suffix(suffix)
{
    setValue(m_defaultValue);
    updateText();
}

void FloatSpinBox::setValue(float value)
{
    float newvalue = BOUND(m_min, value, m_max);
    if (m_value != newvalue)
    {
        m_value = newvalue;
        updateText();
        if (onValueChanged)
            onValueChanged(newvalue);
    }
}

void FloatSpinBox::setRange(float min, float max)
{
    m_min = min;
    m_max = max;
    setValue(m_value);
}

void FloatSpinBox::setMinimum(float value)
{
    m_min = value;
    setValue(m_value);
}

void FloatSpinBox::setMaximum(float value)
{
    m_max = value;
    setValue(m_value);
}

void FloatSpinBox::setStep(float value)
{
    m_step = value;
}

void FloatSpinBox::setDefaultValue(float value)
{
    m_defaultValue = value;
}

void FloatSpinBox::setPrefix(const ByteArray &s)
{
    m_prefix = s;
    updateText();
}

void FloatSpinBox::setSuffix(const ByteArray &s)
{
    m_suffix = s;
    updateText();
}

void FloatSpinBox::setPrecision(int digits)
{
    m_precision = digits;
    updateText();
}

void FloatSpinBox::stepBy(int steps)
{
    if (wrapping() && m_value == m_max && steps > 0)
    {
        setValue(m_min);
    }
    else if (wrapping() && m_value == m_min && steps < 0)
    {
        setValue(m_max);
    }
    setValue(m_value + m_step * steps);
}

void FloatSpinBox::reset()
{
    setValue(m_defaultValue);
}

void FloatSpinBox::updateText()
{
    lineEdit()->setText(m_prefix + ByteArray::number(m_value, 'f', m_precision) + m_suffix);
}
