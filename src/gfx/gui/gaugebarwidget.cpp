#include "gaugebarwidget.h"

GaugeBarWidget::GaugeBarWidget(Widget *parent) :
    GaugeWidget(parent)
{

}

void GaugeBarWidget::paintEvent(Display *d)
{
    int w = width();
    int h = height();

    float percent = 0;
    if (m_maximum > m_minimum)
        percent = (m_value - m_minimum) * 100 / (m_maximum - m_minimum);

    int x = map(m_value);

    // draw background
    d->setBackgroundColor(m_backgroundColor);
    d->fillRoundRect(0, 0, w, h, 3);

    // draw low and high areas
    int lx = 0;
    int hx = w - 2;
    if (m_lowVisible)
        lx = map(m_lowValue);
    if (m_highVisible)
        hx = map(m_highValue);

    if (hx > lx) // adequate check
    {
        Color alarmColor = Red;
        Color constraintColor = Color::blend(Yellow, palette()->disabled(), 32);

        d->setBackgroundColor((m_value < m_lowValue)? alarmColor: constraintColor);
        d->fillRoundRect(1, 1, lx, h-2, 2);
        d->setBackgroundColor((m_value > m_highValue)? alarmColor: constraintColor);
        d->fillRoundRect(hx+1, 1, w-2-hx, h-2, 2);
    }
    else
    {
        lx = 0;
        hx = w - 2;
    }

    Color alarmColor2 = Yellow;

    // draw the indicator
    d->setBackgroundColor(m_color);
//    d->setColor(m_color);
    int N = 3;
    for (int i=-N; i<=N; i++)
    {
        int xi = x + i;
        if (xi < 1 || xi > w-2)
            continue;
        d->setColor(m_color);
        if ((m_value < m_lowValue && xi <= lx) || (m_value > m_highValue && xi >= hx))
            d->setColor(alarmColor2);
        int absi = i<0? -i: i;
        int yi = 3 + (h-2) * absi / (N + 1);
        d->drawLine(xi, yi, xi, h-1);
    }

    d->setColor(palette()->text());
    // draw target ticks
    if (m_targetVisible)
    {
        int tx = map(m_targetValue);
        int N = 3;
        for (int i=-N; i<=N; i++)
        {
            int xi = tx + i;
            if (xi < 1 || xi > w-2)
                continue;
            int absi = i<0? -i: i;
            int yi = 8 * (N - absi + 1) / (N + 1);
            d->drawLine(xi, 1, xi, yi);
        }
//        d->drawTriangle(tx+tw, 1, tx-tw, 1, tx, h/4);

    }

    // draw text
    if (m_textVisible)
    {
        ByteArray s = m_format;
        s.replace("%p", ByteArray::number(percent, 'f', m_decimals));
        s.replace("%v", ByteArray::number(m_value, 'f', m_decimals));
        d->setColor(m_enabled? palette()->text(): palette()->disabledText());
        d->setFont(font());
        d->drawString(0, 0, w, h, AlignCenter, s.data());
    }


    d->setColor(m_borderColor);
    d->drawRoundRect(0, 0, w, h, 3);
}
