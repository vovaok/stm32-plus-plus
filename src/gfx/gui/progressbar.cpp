#include "progressbar.h"
#include "../image.h"
#include <math.h>
#include "core/macros.h"

ProgressBar::ProgressBar(Widget *parent) :
    Widget(parent)
{
    m_color = palette()->accent();
    m_backgroundColor = palette()->base();
    setFixedHeight(font().info().height() + 4);
    update();
}

void ProgressBar::setRange(float minimum, float maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;
    update();
}

void ProgressBar::setMinimum(float value)
{
    if (m_minimum != value)
    {
        m_minimum = value;
        update();
    }
}

void ProgressBar::setMaximum(float value)
{
    if (m_maximum != value)
    {
        m_maximum = value;
        update();
    }
}

void ProgressBar::setValue(float value)
{
    value = BOUND(m_minimum, value, m_maximum);
    if (m_value != value)
    {
        m_value = value;
        update();
    }
}

void ProgressBar::setTextVisible(bool visible)
{
    if (m_textVisible != visible)
    {
        m_textVisible = visible;
        update();
    }
}

void ProgressBar::setFormat(const ByteArray &fmt, int decimals)
{
    m_format = fmt;
    setDecimals(decimals);
    if (m_textVisible)
        update();
}

void ProgressBar::setDecimals(int value)
{
    if (m_decimals != value)
    {
        m_decimals = value;
        if (m_textVisible)
            update();
    }
}

void ProgressBar::update()
{
    updateGeometry();
    Widget::update();
}

void ProgressBar::setFont(Font font)
{
    Widget::setFont(font);
    setFixedHeight(font.info().height() + 4);
}

void ProgressBar::paintEvent(Display *d)
{
    if (d->isReadable())
    {
        doPaint(d);
    }
    else
    {
        Image img(width(), height());
        img.fill(m_parent->backgroundColor());
        doPaint(&img);
        d->drawImage(0, 0, img);
    }
}

void ProgressBar::doPaint(Display *d)
{
    int w = width();
    int h = height();
    int x = 0;
    float percent = 0;
    if (m_maximum > m_minimum)
    {
        x = lrintf((m_value - m_minimum) * (w - 2) / (m_maximum - m_minimum));
        percent = (m_value - m_minimum) * 100 / (m_maximum - m_minimum);
    }

    d->setColor(m_borderColor);
    d->setBackgroundColor(m_backgroundColor);
    d->drawFillRoundRect(0, 0, w, h, 3);
    d->setBackgroundColor(m_color);
    d->fillRoundRect(1, 1, x, h-2, 2);
    if (m_textVisible)
    {
        ByteArray s = m_format;
        s.replace("%p", ByteArray::number(percent, 'f', m_decimals));
        s.replace("%v", ByteArray::number(m_value, 'f', m_decimals));
        d->setColor(m_enabled? palette()->text(): palette()->disabledText());
        d->setFont(font());
        d->drawString(0, 0, w, h, AlignCenter, s.data());
    }
}
