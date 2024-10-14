#include "ledwidget.h"
#include "core/macros.h"

LedWidget::LedWidget(Color color, int size, Widget *parent) :
    Widget(parent),
    m_color(color)
{
    setSize(size);
}

void LedWidget::setSize(int value)
{
    m_img = Image(value, value);
    setFixedSize(value, value);
    update();
}

void LedWidget::setColor(Color color)
{
    m_color = color;
    update();
}

void LedWidget::setState(bool on)
{
    m_state = on;
    update();
}

void LedWidget::toggle()
{
    setState(!m_state);
}

void LedWidget::update()
{
    int d = m_img.width();
    
    Color bgcolor = m_backgroundColor;
    if (parent())
        bgcolor = parent()->backgroundColor();
//    Widget *wid = parent();
//    while (wid && !wid->backgroundColor().alpha())
//        wid = wid->parent();
//    if (wid)
//        bgcolor = wid->backgroundColor();
    
    m_img.fill(bgcolor);
    m_img.setColor(Gray);
    Color back = m_color;
    if (!m_state)
    {
        back = Color::blend(m_color, Black, 64);
        back = Color::blend(back, bgcolor, 64);
    }
    m_img.setBackgroundColor(back);  
//    m_img.drawFillCircle(d/2, d/2, d/2 - 1);
    
    int r = d / 2;
//    int r2 = r * r;
    int r0 = d / 3;
    int r02 = r0 * r0;
    
    int blik = d / 3;
    int b2 = r02;//r * r;
    
    for (int y=0; y<d; y++)
    {
        for (int x=0; x<d; x++)
        {
            int dx = x - r;
            int dy = y - r;
            int dr2 = dx*dx + dy*dy;
            int a;
            if (dr2 <= r02)
            {
                a = 255;
            }
            else
            {
                a = 255 - (dr2 - r02) * 255 / (r02);
            }
            
            int bdx = x - blik;
            int bdy = y - blik;
            int bd2 = bdx*bdx + bdy*bdy;
            int ba = (b2 > bd2)? (b2 - bd2) * 255 / b2: 0;
            ba = ba * ba / 255 / 2;
            
            Color c = back;
            if (a > 0)
            {
                c = Color::blend(back, bgcolor, a);
                c = Color::blend(White, c, ba); // blik
                m_img.setPixelColor(x, y, c);
            }
        }
    }
    
    Widget::update();
}

void LedWidget::paintEvent(Display *d)
{
    if (parent() && m_backgroundColor != parent()->backgroundColor())
        update();
    d->drawImage(0, 0, m_img);
}
