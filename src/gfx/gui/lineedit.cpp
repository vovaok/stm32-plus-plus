#include "lineedit.h"

LineEdit::LineEdit(Widget *parent) :
    Widget(parent)
{
    m_borderRadius = 2;
    setBackgroundColor(White);
    update();
}

LineEdit::LineEdit(const ByteArray &text, Widget *parent) :
    Widget(parent)
{
    m_borderRadius = 2;
    setBackgroundColor(White);
    setText(text);
}

void LineEdit::setAlignment(Alignment value)
{
    m_align = value;
    update();
}

void LineEdit::setText(const ByteArray &text)
{
    m_text = text;
    update();
}

void LineEdit::update()
{
//    setMinimumWidth(textWidth());
    setFixedHeight(textHeight() + 4);
    updateGeometry();
    Widget::update();
}

void LineEdit::paintEvent(Display *d)
{
    d->setColor(m_color);
    d->setBackgroundColor(m_backgroundColor);
    d->setColor(m_borderColor);
    d->drawFillRoundRect(0, 0, m_width, m_height, m_borderRadius);
  
    //Widget::paintEvent(d); // fill background
    if (m_text.size())
    {
        int w = textWidth();
        int h = textHeight();
        Image img(w, h);
        img.fill(m_backgroundColor.rgb565());
        img.setColor(m_color);
        img.setBackgroundColor(m_backgroundColor);
        img.setFont(m_font);
        img.drawString(0, m_font.info().ascent(), m_text.data());
        int xpos=2, ypos=2;
        if (m_align & AlignRight)
            xpos = m_width - w;
        else if (m_align & AlignHCenter)
            xpos = (m_width - w) / 2;        
        d->drawImage(xpos, ypos, img);
    }
}

int LineEdit::textWidth() const
{
    return m_font.info().width(m_text.data());
}

int LineEdit::textHeight() const
{
    return m_font.info().height();
}