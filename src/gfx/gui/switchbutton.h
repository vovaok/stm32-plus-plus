#pragma once

#include "pushbutton.h"

class SwitchButton: public PushButton
{
public:
    SwitchButton(Widget *parent=nullptr);
    std::function<void(bool)> onClick;

    void toggle();
    void setChecked(bool checked);
    bool isChecked() const {return m_checked;}

protected:
    void paintEvent(Display *d);
//    void doPaint(Display *d);
//    void pressEvent(int x, int y);
//    void releaseEvent(int x, int y);

    bool m_checked = false;
};
