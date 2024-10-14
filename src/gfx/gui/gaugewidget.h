#ifndef GAUGEWIDGET_H
#define GAUGEWIDGET_H

#include "progressbar.h"
#include "../image.h"

class GaugeWidget : public ProgressBar
{
public:
    GaugeWidget(Widget *parent=nullptr);
    
    void setBackgroundImage(Image &&img);

    void setTargetValue(float value);
    float targetValue() const {return m_targetValue;}

    void setLowValue(float value);
    float lowValue() const {return m_lowValue;}
    void setHighValue(float value);
    float highValue() const {return m_highValue;}

protected:
    void paintEvent(Display *d) override;
//    void doPaint(Display *d);
    float m_targetValue = 0;
    float m_lowValue = 0;
    float m_highValue = 0;
    bool m_targetVisible = false;
    bool m_lowVisible = false;
    bool m_highVisible = false;

private:
    Image m_back;
    void drawBack();
};

#endif // GAUGEWIDGET_H
