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
    Image img = m_image;
    Color fgcol = m_enabled? m_color: palette()->disabledText();
    Color bgcol = m_enabled? m_backgroundColor: palette()->disabled();
    if (m_text.size())
    {
        int w = textWidth();
        int h = textHeight();
        img = Image(w, h, Format_ARGB4444);//d->pixelFormat());

//        if (m_parent)
//            img.fill(m_parent->backgroundColor());
//        else
//            img.fill(bgcol);
        img.fill(Transparent);
        img.setColor(fgcol);
//        img.setBackgroundColor(bgcol);
        img.setFont(font());
        img.drawString(2, 2+font().info().ascent(), m_text.data());
    }
    if (!img.isNull())
    {
        int w = img.width();
        int h = img.height();
        int xpos=0, ypos=0;
        if (m_align & AlignRight)
            xpos = m_width - w;
        else if (m_align & AlignHCenter)
            xpos = (m_width - w) / 2;
        if (m_align & AlignBottom)
            ypos = m_height - h;
        else if (m_align & AlignVCenter)
            ypos = (m_height - h) / 2;
//        Widget::paintEvent(d); // fill background
//        dynamic_cast<FrameBuffer*>(d)->setOpacity(m_opacity);
        d->setColor(fgcol);
        d->setBackgroundColor(bgcol);
        if (m_borderSize == 1)
            d->drawFillRoundRect(0, 0, m_width, m_height, m_borderRadius);
        else
            d->fillRoundRect(0, 0, m_width, m_height, m_borderRadius);
        img.setOpacity(m_opacity);
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
