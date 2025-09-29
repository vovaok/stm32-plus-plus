#pragma once

#include "widget.h"

class ProgressBar : public Widget
{
public:
    ProgressBar(Orientation orient, Widget *parent = nullptr);
    ProgressBar(Widget *parent = nullptr) : ProgressBar(Horizontal, parent) {}

    void setRange(float minimum, float maximum);
    void setMinimum(float value);
    float minimum() const {return m_minimum;}
    void setMaximum(float value);
    float maximum() const {return m_maximum;}

    void setValue(float value);
    inline float value() const {return m_value;}

    void setOrientation(Orientation orient);
    Orientation orientation() const {return m_orientation;}
    void setTextVisible(bool visible);
    void setFormat(const ByteArray &fmt);
    const ByteArray &format() const {return m_format;}
    void setDecimals(int value);
    int decimals() const {return m_decimals;}

    void setFont(Font font);
    
    void useColorMap(Gradient *gradient);

protected:
    void paintEvent(Display *d) override;
    void doPaint(Display *d);

    float m_value = 0;
    float m_minimum = 0;
    float m_maximum = 100;
    int m_decimals = 0;
    bool m_textVisible = true;
    ByteArray m_format = "%p %";
    Orientation m_orientation;
    Gradient *m_colorMap = nullptr;

    //! @return x-coord of value in px;
    int map(float value);

//private:

};
