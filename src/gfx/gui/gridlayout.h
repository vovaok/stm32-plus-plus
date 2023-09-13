#pragma once

#include "widget.h"

class GridLayout : public Layout
{
public:
    GridLayout();
    
    void addWidget(Widget *w, int row, int col);
    Widget *widgetAt(int row, int col);
    
    void update();
    
private:
    struct Item
    {
        Widget *w;
        int row;
        int column;
    };
    
    std::vector<Item> m_items;
    int m_rows=0, m_cols=0;
};

void GridLayout::addWidget(Widget *w, int row, int column)
{
    Item item = {w, row, col};
    if (m_rows <= row)
        m_rows = row + 1;
    if (m_cols <= col)
        m_cols = col + 1;
}

void GridLayout::widgetAt(int row, int col)
{
    for (Item &item: m_items)
    {
        if (item.row == row && item.col == col)
            return item.w;
    }
    return nullptr;
}

void GridLayout::update()
{
    int maxw = 0;
    int maxh = 0;
    
    for (int j=0; j<m_cols; j++)
    {
        int maxw = 0;
        for (int i=0; i<m_rows; i++)
        {
            Widget *w = widgetAt(i, j);
            if (!w)
                continue;
            if (maxw < w->width())
                maxw = w->width();
        }
    }
}