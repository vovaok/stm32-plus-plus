#include "gaugewidget.h"
#include <math.h>
#include "core/advmath.h"
#include "core/macros.h"

GaugeWidget::GaugeWidget(Widget *parent) :
    ProgressBar(parent)
{
}

void GaugeWidget::setBackgroundImage(Image &&img)
{
    m_back = std::move(img);
}

void GaugeWidget::setTargetValue(float value)
{
    bool needUpdate = !m_targetVisible || (m_targetValue != value);
    m_targetValue = BOUND(m_minimum, value, m_maximum);
    m_targetVisible = true;
    if (needUpdate)
        update();
}

void GaugeWidget::paintEvent(Display *d)
{
    int w = width();
    int h = height();

    if (h > w/2 + 6)
        h = w/2 + 6;

//    if (w != m_back.width() || h != m_back.height())
    if (m_back.isNull()) // draw background image only once!
        drawBack();

    int a = 0;
    float percent = 0;
    if (m_maximum > m_minimum)
    {
//        a = (m_maximum - m_value) * 180 / (m_maximum - m_minimum);
        a = static_cast<int>((m_maximum - m_value) * 128 / (m_maximum - m_minimum));
        percent = (m_value - m_minimum) * 100 / (m_maximum - m_minimum);
    }

    d->drawImage(0, 0, m_back);

    int cx = w / 2;
    int cy = h - 6;
    int r = cx - 6;

//    // Bhaskara sine approximation
//    auto sin_deg = [](int x, int r)
//    {
//        x = x % 360;
//        if (x < 0)
//            x += 360;
//        if (x < 180)
//            return 4*r*x*(180-x) / (40500-x*(180-x));
//        x -= 180;
//            return -4*r*x*(180-x) / (40500-x*(180-x));
//    };

//    auto cos_deg = [sin_deg](int x, int r)
//    {
//        return sin_deg(x + 90, r);
//    };

    if (m_targetVisible)
    {
//        int ta = (m_maximum - m_targetValue) * 180 / (m_maximum - m_minimum);
//        int x = lroundf(cx + r*cos(ta));
//        int y = lroundf(cy - r*sin(ta));
//        int x = cx + sin_deg(ta+90, r);
//        int y = cy - sin_deg(ta, r);
        int ta = static_cast<int>((m_maximum - m_targetValue) * 128 / (m_maximum - m_minimum));
        float cta = cos_i16(ta) / 32768.f;
        float sta = sin_i16(ta) / 32768.f;
        float r1 = r;// * 0.9f;
        float r2 = cx;// * 1.1f;
        int x = static_cast<int>(cx + r1 * cta);
        int y = static_cast<int>(cy - r1 * sta);
        int x1 = static_cast<int>(cx + r2 * cta - 2*sta);
        int y1 = static_cast<int>(cy - r2 * sta - 2*cta);
        int x2 = static_cast<int>(cx + r2 * cta + 2*sta);
        int y2 = static_cast<int>(cy - r2 * sta + 2*cta);

        d->setColor(Black);
        d->drawTriangle(x, y, x1, y1, x2, y2);
        
//        d->setBackgroundColor(Green);
//        d->fillCircle(x, y, 4);
    }

    int rx = r * cos_i16(a) / 32768;
    int ry = r * sin_i16(a) / 32768;
    int x = cx + rx;
    int y = cy - ry;

    r = r * 100 / 75;

    d->setColor(m_color);
    d->setBackgroundColor(m_color);
//    d->drawTriangle();
    for (int i=-3; i<=3; i++)
        d->drawLine(cx + i*ry/r, cy + i*rx/r, x, y);
    d->setColor(m_backgroundColor);
    d->drawFillCircle(cx, cy, 4);

    if (m_textVisible)
    {
        ByteArray s = m_format;
        s.replace("%p", ByteArray::number(percent, 'f', m_decimals));
        s.replace("%v", ByteArray::number(m_value, 'f', m_decimals));
        d->setColor(m_enabled? palette()->text(): palette()->disabledText());
        d->setFont(font());
        d->drawString(0, 0, w, h, AlignCenter, s.data());
    }
}

void GaugeWidget::drawBack()
{
    if (!parent())
        return;

    int w = width();
    int h = height();
    if (h > w/2 + 6)
        h = w/2 + 6;

    m_back = Image(w, h, Format_RGB565);
//    m_back.fill(Transparent);
    m_back.fill(parent()->backgroundColor());
    m_back.setColor(m_borderColor);
    m_back.setBackgroundColor(m_backgroundColor);
    m_back.drawFillCircle(w/2, h-5, w/2-1);
    m_back.drawLine(1, h-1, w, h-1);
}
