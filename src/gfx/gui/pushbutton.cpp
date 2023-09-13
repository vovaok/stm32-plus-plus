#include "pushbutton.h"

PushButton::PushButton(const ByteArray &text, Widget *parent) :
    Widget(parent)
{
    setBackgroundColor(Color(160, 160, 160));
    setText(text);
}

void PushButton::setText(const ByteArray &text)
{
    m_text = text;
    update();
}

//void PushButton::update()
//{
//    setMinimumWidth(textWidth());
//    setMinimumHeight(textHeight());
//    updateGeometry();
//    Widget::update();
//}

void PushButton::paintEvent(Display *d)
{
    if (m_text.size())
    {
        int w = width();
        int h = height();
        Image img(w, h);
        img.fill(parent()->backgroundColor().rgb565()); // a la transparent background
        img.setColor(m_color);
        img.setBackgroundColor(m_backgroundColor);
        img.setFont(m_font);
        img.drawFillRoundRect(1, 1, w-2, h-2, 4);
        img.drawString(0, 0, w, h, AlignCenter | TextWordWrap, m_text.data());
//        Widget::paintEvent(d); // fill background
        d->drawImage(0, 0, img);
    }
}

int PushButton::textWidth() const
{
    return m_font.info().width(m_text.data()) + 4;
}

int PushButton::textHeight() const
{
    return m_font.info().height() + 4;
}