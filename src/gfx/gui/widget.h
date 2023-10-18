#pragma once

#include "../display.h"
#include "../touchscreen.h"
//#include "layout.h"

class Layout;

class Widget
{
public:
    Widget(Widget *parent=nullptr);

    void setParent(Widget *w);
    void addWidget(Widget *w);
    void removeWidget(Widget *w);

    Widget *parent() {return m_parent;}
    Widget *child(int idx);
    int childrenCount() const {return m_children.size();}

    void setLayout(Layout *layout);
    Layout *layout() {return m_layout;}

    int x() const {return m_x;}
    int y() const {return m_y;}
    int width() const {return m_width;}
    int height() const {return m_height;}

    void move(int x, int y);
    void resize(int width, int height);
    void setWidth(int value);
    void setHeight(int value);
    void setGeometry(int x, int y, int w, int h);

    void setMinimumWidth(int value) {m_minWidth = value;}
    int minimumWidth() const {return m_minWidth;}
    void setMinimumHeight(int value) {m_minHeight = value;}
    int minimumHeight() const {return m_minHeight;}
    void setMinimumSize(int width, int height);
    void setMaximumWidth(int value) {m_maxWidth = value;}
    int maximumWidth() const {return m_maxWidth;}
    void setMaximumHeight(int value) {m_maxHeight = value;}
    int maximumHeight() const {return m_maxHeight;}
    void setMaximumSize(int width, int height);
    void setFixedWidth(int value);
    void setFixedHeight(int value);
    void setFixedSize(int width, int height);

    void setVisible(bool visible);
    bool visible() const {return m_visible;}

    bool hasFocus() const;
    void setFocus();
    void clearFocus();

    void update();
    void updateGeometry();

    void setBackgroundColor(Color color);
    Color backgroundColor() const {return m_backgroundColor;}
    void setColor(Color color);
    Color color() const {return m_color;}
    void setFont(Font font);
    Font font() const {return m_font;}

    Widget *widgetAt(int x, int y);

protected:
    int m_x = 0, m_y = 0;
    int m_width = 0, m_height = 0;
    int m_minWidth = 0, m_minHeight = 0;
    int m_maxWidth = 9999, m_maxHeight = 9999;
    bool m_visible = true;
    bool m_acceptTouchEvents = false;

    // style
    Color m_backgroundColor = Color(224, 224, 224);
    Color m_color;
    Font m_font;
    Color m_borderColor = Gray;
    int m_borderRadius = 0;

    Widget *m_parent = nullptr;
    std::vector<Widget *> m_children;
    Layout *m_layout = nullptr;
    friend class Layout;

    virtual void paintEvent(Display *d);

    virtual void pressEvent(int x, int y) {}
    virtual void moveEvent(int x, int y) {}
    virtual void releaseEvent(int x, int y) {}

private:
    bool m_needRepaint = true;

    friend class GuiApplication;
    void paint(Display *d);
};