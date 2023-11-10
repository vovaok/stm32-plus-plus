#include "spinbox.h"
#include "core/macros.h"
#include "flexlayout.h"
#include "pushbutton.h"

SpinBox::SpinBox(Widget *parent) : AbstractSpinBox(parent)
{
    setValue(0);
}

void SpinBox::setValue(int value)
{
    int newvalue = BOUND(m_min, value, m_max);
    if (m_value != newvalue)
    {
        m_value = newvalue;
        updateText();
        if (onValueChanged)
            onValueChanged(newvalue);
    }
}

void SpinBox::setRange(int min, int max)
{
    m_min = min;
    m_max = max;
    setValue(m_value);
}

void SpinBox::setMinimum(int value)
{
    m_min = value;
    setValue(m_value);
}

void SpinBox::setMaximum(int value)
{
    m_max = value;
    setValue(m_value);
}

void SpinBox::setStep(int value)
{
    m_step = value;
}

void SpinBox::setPrefix(const ByteArray &s)
{
    m_prefix = s;
    updateText();
}

void SpinBox::setSuffix(const ByteArray &s)
{
    m_suffix = s;
    updateText();
}

void SpinBox::stepBy(int steps)
{
    setValue(m_value + m_step * steps);
}

void SpinBox::updateText()
{
    lineEdit()->setText(m_prefix + ByteArray::number(m_value) + m_suffix);
}
