#include "progressbar.h"
#include "../image.h"
#include "core/macros.h"

ProgressBar::ProgressBar(Orientation orient, Widget *parent) :
    Widget(parent),
    m_orientation(orient)
{
    m_color = palette()->accent();
    m_backgroundColor = palette()->base();
    int def_sz = font().info().height() + 4;
    switch (m_orientation)
    {
    case Horizontal: case HorizontalReversed: setFixedHeight(def_sz); break;
    case Vertical:   case VerticalReversed:   setFixedWidth (def_sz); break;
    }
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

void ProgressBar::setFormat(const ByteArray &fmt)
{
    if (m_format != fmt)
    {
        m_format = fmt;
        if (m_textVisible)
            update();
    }
}

//void ProgressBar::setFormat(const ByteArray &fmt, int decimals)
//{
//    m_format = fmt;
//    setDecimals(decimals);
//    if (m_textVisible)
//        update();
//}

void ProgressBar::setDecimals(int value)
{
    if (m_decimals != value)
    {
        m_decimals = value;
        if (m_textVisible)
            update();
    }
}

void ProgressBar::setFont(Font font)
{
    Widget::setFont(font);
    setFixedHeight(font.info().height() + 4);
    updateGeometry();
}

void ProgressBar::setOrientation(Orientation orient)
{
    if (m_orientation != orient)
    {
        m_orientation = orient;
        update();
    }
}

void ProgressBar::useColorMap(Gradient *gradient)
{
    m_colorMap = gradient;
    update();
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
    int p = 0; // x or y
    float percent = 0;
    if (m_maximum > m_minimum)
    {
        p = map(m_value);
        percent = (m_value - m_minimum) * 100 / (m_maximum - m_minimum);
    }
    else
    {
        //! @todo implement indeterminate state appearance
    }

    d->setColor(m_borderColor);
    d->setBackgroundColor(m_backgroundColor);
    d->drawFillRoundRect(0, 0, w, h, 3);
    if (m_colorMap)
        d->setBackgroundColor(m_colorMap->colorAt(percent * 0.01f));
    else
        d->setBackgroundColor(m_color);
    
    switch (m_orientation)
    {
    case Horizontal:         d->fillRoundRect(1,   1,   p-1, h-2, 2); break;
    case Vertical:           d->fillRoundRect(1,   h-p, w-2, p-1, 2); break;
    case HorizontalReversed: d->fillRoundRect(w-p, 1,   p-1, h-2, 2); break;
    case VerticalReversed:   d->fillRoundRect(1,   1,   w-2, p-1, 2); break;
    }
    
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

int ProgressBar::map(float value)
{
    if (m_maximum == m_minimum)
        return 0;
    int sz;
    switch (m_orientation)
    {
    case Horizontal: case HorizontalReversed: sz = m_width  - 2; break;
    case Vertical:   case VerticalReversed:   sz = m_height - 2; break;
    }
    
    int x = static_cast<int>((value - m_minimum) * sz / (m_maximum - m_minimum)) + 1;
    
    if (x < 1)
        return 1;
    else if (x > sz)
        return sz;
    return x;
}
