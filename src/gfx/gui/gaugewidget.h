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

protected:
    void paintEvent(Display *d) override;
//    void doPaint(Display *d);

private:
    Image m_back;
    float m_targetValue = 0;
    bool m_targetVisible = false;

    void drawBack();
};

#endif // GAUGEWIDGET_H
