#include "label.h"

Label::Label(Widget *parent) :
    Widget(parent)
{
    setFixedHeight(textHeight());
}

Label::Label(const ByteArray &text, Widget *parent) :
    Widget(parent),
    m_text(text)
{
    setFixedHeight(textHeight());
    update();
}

Label::Label(const ByteArray &text, Alignment align, Widget *parent) :
    Widget(parent),
    m_text(text),
    m_align(align)
{
    setFixedHeight(textHeight());
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

void Label::setImage(const Image &img)
{
    m_image = img;
    update();
}

void Label::setFont(Font font)
{
    Widget::setFont(font);
    setMinimumHeight(textHeight());
}

void Label::update()
{
    setMinimumWidth(textWidth());
//    setFixedHeight(textHeight());
    updateGeometry();
    Widget::update();
}

void Label::paintEvent(Display *d)
{
    if (m_text.size())
    {
        int w = textWidth();
        int h = textHeight();
        m_image = Image(w, h, d->pixelFormat());
        Color bgcol = m_enabled? m_backgroundColor: palette()->disabled();
        m_image.fill(bgcol);
        m_image.setColor(m_enabled? m_color: palette()->disabledText());
        m_image.setFont(font());
        m_image.drawString(2, 2+font().info().ascent(), m_text.data());
    }
    if (!m_image.isNull())
    {
        int w = m_image.width();
        int h = m_image.height();
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
        d->drawImage(xpos, ypos, m_image);
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
