#include "label.h"

Label::Label(Widget *parent) :
    Widget(parent)
{
}

Label::Label(const ByteArray &text, Widget *parent) :
    Widget(parent),
    m_text(text)
{
    update();
}

void Label::setAlignment(Alignment value)
{
    m_align = value;
    update();
}

void Label::setText(const ByteArray &text)
{
    if (m_text != text)
    {
        m_text = text;
        update();
    }
}

void Label::setNum(int n)
{
    setText(ByteArray::number(n));
}

void Label::setNum(float n, int prec)
{
    setText(ByteArray::number(n, 'f', prec));
}

void Label::update()
{
    setMinimumWidth(textWidth());
    setFixedHeight(textHeight());
    updateGeometry();
    Widget::update();
}

void Label::paintEvent(Display *d)
{
    //Widget::paintEvent(d); // fill background
    if (m_text.size())
    {
        int w = textWidth();
        int h = textHeight();
        Image img(w, h);
        Color bgcol = m_enabled? m_backgroundColor: palette()->disabled();
        img.fill(bgcol.rgb565());
        img.setColor(m_enabled? m_color: palette()->disabledText());
        img.setFont(font());
        img.drawString(2, 2+font().info().ascent(), m_text.data());
        int xpos=0, ypos=0;
        if (m_align & AlignRight)
            xpos = m_width - w;
        else if (m_align & AlignHCenter)
            xpos = (m_width - w) / 2;
        if (m_align & AlignBottom)
            ypos = m_height - h;
        else if (m_align & AlignVCenter)
            ypos = (m_height - h) / 2;
        Widget::paintEvent(d); // fill background
        d->drawImage(xpos, ypos, img);
    }
}

int Label::textWidth() const
{
    return font().info().width(m_text.data()) + 4;
}

int Label::textHeight() const
{
    return font().info().height() + 4;
}
