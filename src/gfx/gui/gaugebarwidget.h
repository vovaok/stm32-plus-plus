#ifndef GAUGEBARWIDGET_H
#define GAUGEBARWIDGET_H

#include "gaugewidget.h"

class GaugeBarWidget : public GaugeWidget
{
public:
    GaugeBarWidget(Widget *parent=nullptr);

protected:
    void paintEvent(Display *d) override;
};

#endif // GAUGEBARWIDGET_H
