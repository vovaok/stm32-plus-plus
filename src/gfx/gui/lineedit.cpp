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
    if (value != m_align)
    {
        m_align = value;
        update();
    }
}

void LineEdit::setText(const ByteArray &text)
{
    if (text != m_text)
    {
        m_text = text;
        update();
    }
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
//    d->setColor(m_color);
    Color bgcol = m_enabled? m_backgroundColor: palette()->disabled();
    Color col = m_enabled? m_color: palette()->disabledText();

    d->setBackgroundColor(bgcol);
    d->setColor(col);
    d->drawFillRoundRect(0, 0, m_width, m_height, m_borderRadius);

    //Widget::paintEvent(d); // fill background
    if (m_text.size())
    {
        int w = textWidth();
        int h = textHeight();
        if (d->isReadable())
        {
            d->setFont(font());
            int xpos=2, ypos=2;
            if (m_align & AlignRight)
                xpos = m_width - w;
            else if (m_align & AlignHCenter)
                xpos = (m_width - w) / 2;
            d->drawString(xpos, ypos + font().info().ascent(), m_text.data());
        }
        else
        {
            Image img(w, h);
            img.fill(bgcol);
            img.setColor(col);
            img.setBackgroundColor(bgcol);
            img.setFont(font());
            img.drawString(0, font().info().ascent(), m_text.data());
            int xpos=2, ypos=2;
            if (m_align & AlignRight)
                xpos = m_width - w;
            else if (m_align & AlignHCenter)
                xpos = (m_width - w) / 2;
            d->drawImage(xpos, ypos, img);
        }
    }
}

int LineEdit::textWidth() const
{
    return font().info().width(m_text.data());
}

int LineEdit::textHeight() const
{
    return font().info().height();
}
