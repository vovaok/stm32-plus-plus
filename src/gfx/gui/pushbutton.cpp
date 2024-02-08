#include "pushbutton.h"

PushButton::PushButton(const ByteArray &text, Widget *parent) :
    Widget(parent)
{
    m_acceptTouchEvents = true;
    m_borderRadius = 4;
    m_upColor = palette()->button();
    m_downColor = palette()->dark();
    setBackgroundColor(m_upColor);
    setText(text);
}

void PushButton::setText(const ByteArray &text)
{
    if (m_text != text)
    {
        m_text = text;
        update();
    }
}

void PushButton::click()
{
    if (onClick && m_enabled)
        onClick();
}

void PushButton::setDown(bool value)
{
    m_down = value;
    if (value)
        setBackgroundColor(m_downColor);
    else
        setBackgroundColor(m_upColor);
}

void PushButton::setColor(Color c, uint8_t saturation)
{
    m_upColor = Color::blend(c, palette()->button(), saturation);
    m_downColor = Color::blend(c, palette()->dark(), saturation);
    setBackgroundColor(m_upColor);
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
        img.fill(parent()->backgroundColor()); // a la transparent background
        int off = 0;
        if (m_down)
            off = 1;
        img.setBackgroundColor(m_enabled? m_backgroundColor: palette()->disabled());
        img.setFont(font());
        img.setColor(m_enabled? m_borderColor: palette()->disabledText());
        img.drawFillRoundRect(off, off, w-off, h-off, m_borderRadius);
        img.setColor(m_enabled? m_color: palette()->disabledText());
        img.drawString(off, off, w-off*2, h-off*2, (int)AlignCenter | (int)TextWordWrap, m_text.data());
//        Widget::paintEvent(d); // fill background
        d->drawImage(0, 0, img);
    }
}

void PushButton::pressEvent(int x, int y)
{
    setDown(true);
//    update();
}

void PushButton::releaseEvent(int x, int y)
{
    if (m_down)
    {
        setDown(false);
        update();
        if (onClick)
            onClick();
    }
}

int PushButton::textWidth() const
{
    return font().info().width(m_text.data()) + 4;
}

int PushButton::textHeight() const
{
    return font().info().height() + 4;
}
