#pragma once

class RectF
{
public:
    RectF() : m_x(0), m_y(0), m_w(0), m_h(0) {}

    RectF(float x, float y, float width, float height):
        m_x(x), m_y(y), m_w(width), m_h(height)
    {
    }

    RectF(const Rect &rect) :
        m_x(rect.x()), m_y(rect.y()), m_w(rect.width()), m_h(rect.height())
    {
    }

    float x() const {return m_x;}
    void setX(float value) {m_x = value;}
    float y() const {return m_y;}
    void setY(float value) {m_y = value;}
    float width() const {return m_w;}
    void setWidth(float value) {m_w = value;}
    float height() const {return m_h;}
    void setHeight(float value) {m_h = value;}

    void adjust(float dx1, float dy1, float dx2, float dy2)
    {
        m_x += dx1;
        m_y += dy1;
        m_w += dx2 - dx1;
        m_h += dy2 - dy1;
    }

    RectF adjusted(float dx1, float dy1, float dx2, float dy2)
    {
        return RectF(m_x+dx1, m_y+dy1, m_w+dx2-dx1, m_h+dy2-dy1);
    }

    Rect toRect()
    {
        return Rect(lroundf(m_x), lroundf(m_y), lroundf(m_w), lroundf(m_h));
    }

private:
    float m_x, m_y, m_w, m_h;
};