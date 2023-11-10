#pragma once

class Rect
{
public:
    Rect() : m_x(0), m_y(0), m_w(0), m_h(0) {}
    
    Rect(int x, int y, int width, int height):
        m_x(x), m_y(y), m_w(width), m_h(height)
    {
    }
    
    int x() const {return m_x;}
    void setX(int value) {m_x = value;}
    int y() const {return m_y;}
    void setY(int value) {m_y = value;}
    int width() const {return m_w;}
    void setWidth(int value) {m_w = value;}
    int height() const {return m_h;}
    void setHeight(int value) {m_h = value;}
    
    void adjust(int dx1, int dy1, int dx2, int dy2)
    {
        m_x += dx1;
        m_y += dy1;
        m_w += dx2 - dx1;
        m_h += dy2 - dy1;
    }
    
    Rect adjusted(int dx1, int dy1, int dx2, int dy2)
    {
        return Rect(m_x+dx1, m_y+dy1, m_w+dx2-dx1, m_h+dy2-dy1);
    }
      
private:
    int m_x, m_y, m_w, m_h;
};