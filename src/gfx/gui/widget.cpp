#include "widget.h"
#include "layout.h"
#include "guiapplication.h"

Widget::Widget(Widget *parent)
{
//    if (!parent)
//        parent = GuiApplication::widget();
    setParent(parent);
}

void Widget::setParent(Widget *parent)
{
    if (m_parent)
        m_parent->removeWidget(this);
    m_parent = parent;
    if (m_parent)
        m_parent->addWidget(this);
}

void Widget::addWidget(Widget *w)
{
    m_children.push_back(w);
    w->m_parent = this;
//    w->m_backgroundColor = m_backgroundColor;
    updateGeometry();
}

void Widget::removeWidget(Widget *w)
{
    auto it = std::find(m_children.begin(), m_children.end(), w);
    // if (it != m_children.end())
    m_children.erase(it);
    updateGeometry();
}

Widget *Widget::child(int idx)
{
    if (idx >= 0 && idx < m_children.size())
        return m_children.at(idx);
    return nullptr;
}

void Widget::setLayout(Layout *layout)
{
    if (m_layout)
        m_layout->m_widget = nullptr;
    delete m_layout;
    m_layout = layout;
    m_layout->m_widget = this;
    updateGeometry();
}

void Widget::move(int x, int y)
{
    setGeometry(x, y, m_width, m_height);
}

void Widget::resize(int width, int height)
{
    setGeometry(m_x, m_y, width, height);
}

void Widget::setWidth(int value)
{
    setGeometry(m_x, m_y, value, m_height);
}

void Widget::setHeight(int value)
{
    setGeometry(m_x, m_y, m_width, value);
}

void Widget::setGeometry(int x, int y, int w, int h)
{
    if (m_parent && m_parent->m_layout)
        return; // layout manages the widget's size
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
    updateGeometry();
//    if (m_parent)
//        m_parent->update();
}

void Widget::setMinimumSize(int width, int height)
{
    m_minWidth = width;
    m_minHeight = height;
}

void Widget::setMaximumSize(int width, int height)
{
    m_maxWidth = width;
    m_maxHeight = height;
}

void Widget::setFixedWidth(int value)
{
    m_minWidth = m_maxWidth = value;
}

void Widget::setFixedHeight(int value)
{
    m_minHeight = m_maxHeight = value;
}

void Widget::setFixedSize(int width, int height)
{
    m_minWidth = m_maxWidth = width;
    m_minHeight = m_maxHeight = height;
}

void Widget::setVisible(bool visible)
{
    if (m_visible != visible)
    {
        m_visible = visible;
        if (m_parent)
            m_parent->update();
    }
}

void Widget::setEnabled(bool enabled)
{
    if (m_enabled != enabled)
    {
        m_enabled = enabled;
        for (Widget *w: m_children)
            w->setEnabled(enabled);
        update();
    }
}

bool Widget::hasFocus() const
{
    return this == GuiApplication::focusWidget();
}

void Widget::setFocus()
{
    GuiApplication::setFocusWidget(this);
    update();
}

void Widget::clearFocus()
{
    if (hasFocus())
    {
        GuiApplication::setFocusWidget(nullptr);
        update();
    }
}

void Widget::update()
{
    m_needRepaint = true;
    for (Widget *w: m_children)
    {
        if (w)
            w->update();
//        w->m_needRepaint = true;
    }
}

void Widget::updateGeometry()
{
    for (Widget *w = this; w; w = w->m_parent)
    {
        if (w->m_layout)
            w->m_layout->m_needUpdate = true;
    }
}

const Palette *Widget::palette() const
{
    return GuiApplication::palette();
}

void Widget::setBackgroundColor(Color color)
{
    if (m_backgroundColor != color)
    {
        m_backgroundColor = color;
        update();
    }
}

void Widget::setColor(Color color)
{
    if (m_color != color)
    {
        m_color = color;
        update();
    }
}

void Widget::setFont(Font font)
{
//    if (m_font != font)
    {
        m_font = font;
        update();
    }
}

Widget *Widget::widgetAt(int x, int y)
{
//    int x0 = x - m_x;
//    int y0 = y - m_y;
    for (Widget *w: m_children)
    {
        if (!w->m_visible)
            continue;
        int rx = x - w->m_x;
        int ry = y - w->m_y;
        if (rx >= 0 && rx < w->m_width &&
            ry >= 0 && ry < w->m_height)
        {
            return w->widgetAt(rx, ry);
        }
    }
    return this;
}

void Widget::paintEvent(Display *d)
{
    // fill background
    d->setBackgroundColor(m_backgroundColor);
    d->fillRect(0, 0, m_width, m_height);
}

void Widget::paint(Display *d)
{
    if (!m_visible)
        return;

    if (m_layout && m_layout->m_needUpdate)
    {
        m_layout->m_needUpdate = false;
        m_layout->update();
    }

    int oldx = d->xPos();
    int oldy = d->yPos();
    d->moveTo(oldx + m_x, oldy + m_y);

    if (m_needRepaint)
        paintEvent(d);
    m_needRepaint = false;

    // recursive repaint child widgets
    for (Widget *w: m_children)
        w->paint(d);

    if (hasFocus())
    {
        Color c2 = palette()->accent();
        Color c1 = Color::blend(c2, m_backgroundColor, 128);
        Color c0 = Color::blend(c2, m_backgroundColor, 64);
        d->setColor(c0);
        d->drawRoundRect(0, 0, m_width, m_height, 5);
        d->setColor(c1);
        d->drawRoundRect(1, 1, m_width-2, m_height-2, 4);
        d->setColor(c2);
        d->drawRoundRect(2, 2, m_width-4, m_height-4, 3);
    }

    d->moveTo(oldx, oldy);
}
