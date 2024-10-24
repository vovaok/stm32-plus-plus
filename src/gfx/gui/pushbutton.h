#pragma once

#include "widget.h"
#include "../image.h"
#include "core/coretypes.h"
#include <functional>

class PushButton: public Widget
{
public:
    PushButton(const ByteArray &text="", Widget *parent=nullptr);

    void setText(const ByteArray &text);
    const ByteArray &text() const {return m_text;}

    std::function<void(void)> onClick;
    void click();

    bool isDown() const {return m_down;}
    void setDown(bool value);

    void setColor(Color c, uint8_t saturation=64);

//    void update();

protected:
    void paintEvent(Display *d);
    void doPaint(Display *d);
    void pressEvent(int x, int y);
    void releaseEvent(int x, int y);

private:
    ByteArray m_text;
    bool m_down = false;
    Color m_upColor;
    Color m_downColor;

    int textWidth() const;
    int textHeight() const;
};
