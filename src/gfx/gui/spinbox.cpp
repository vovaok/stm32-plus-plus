#include "spinbox.h"
#include "core/macros.h"
#include "flexlayout.h"
#include "pushbutton.h"

SpinBox::SpinBox(Widget *parent) : Widget(parent)
{
    const FontInfo &fi = m_font.info();
    setFixedHeight(fi.height() + 4);
    int bw = fi.width('<') * 2;
    PushButton *btnDown = new PushButton("<");
    btnDown->setFixedWidth(bw);
    btnDown->onClick = EVENT(&SpinBox::stepDown);
    PushButton *btnUp = new PushButton(">");
    btnUp->setFixedWidth(bw);
    btnUp->onClick = EVENT(&SpinBox::stepUp);
    m_edit = new LineEdit("0");
    m_edit->setAlignment(AlignCenter);
    addWidget(btnDown);
    addWidget(m_edit);
    addWidget(btnUp);
    setLayout(new FlexLayout(Horizontal));
}

void SpinBox::setValue(int value)
{
    int newvalue = BOUND(m_min, value, m_max);
    if (m_value != newvalue)
    {
        m_value = newvalue;
        updateText();
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

void SpinBox::stepUp()
{
    stepBy(1);
}

void SpinBox::stepDown()
{
    stepBy(-1);
}

//void SpinBox::paintEvent(Display *d)
//{
//    
//}

void SpinBox::stepBy(int steps)
{
    setValue(m_value + m_step * steps);
}

void SpinBox::updateText()
{
    m_edit->setText(m_prefix + ByteArray::number(m_value) + m_suffix);
}
