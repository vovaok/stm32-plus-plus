#include "switchbutton.h"

SwitchButton::SwitchButton(Widget *parent) :
    PushButton("", parent)
{
    PushButton::onClick = std::bind(&SwitchButton::toggle, this);
}

void SwitchButton::toggle()
{
    setChecked(!m_checked);
}

void SwitchButton::setChecked(bool checked)
{
    if (m_checked != checked)
    {
        m_checked = checked;
        update();
        if (onClick)
            onClick(m_checked);
    }
}

void SwitchButton::paintEvent(Display *d)
{
    int w = width();
    int h = height();
//    int off = h / 8;
    if (m_enabled)
        d->setBackgroundColor(m_checked? palette()->accent(): m_backgroundColor);
    else
        d->setBackgroundColor(palette()->disabled());
//    d->setFont(font());
    d->setColor(m_enabled? m_borderColor: palette()->disabledText());
//    d->drawFillRoundRect(off, off, w-off*2, h-off*2, h/2-off);
    d->drawFillRoundRect(rect(), h/2);
    d->setBackgroundColor(m_enabled? palette()->window(): palette()->disabled());
    if (!m_checked)
        d->drawFillCircle(h/2, h/2, h/3);
    else
        d->drawFillCircle(w-h/2, h/2, h/3);
//    d->setColor(m_enabled? m_color: palette()->disabledText());
//    d->drawString(off, off, w-off*2, h-off*2, (int)AlignCenter | (int)TextWordWrap, m_text.data());
}
