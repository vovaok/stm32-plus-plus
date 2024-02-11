#pragma once

#include "layout.h"
#include "core/macros.h"

class FlexLayout : public Layout
{
public:  
    enum JustifyContent
    {
        Justify_FlexStart   = 0x0001,
        Justify_FlexEnd     = 0x0002,
        Justify_Center      = 0x0003,
        Justify_SpaceBetween= 0x0004, 
        Justify_SpaceAround = 0x0005,
        Justify_SpaceEvenly = 0x0006,
        JustifyContent_Mask = 0x0007
    };
    
    enum AlignItems
    {
        Align_FlexStart     = 0x0010,
        Align_FlexEnd       = 0x0020,
        Align_Center        = 0x0030,
        Align_Stretch       = 0x0040,
        AlignItems_Mask     = 0x0070
    };
  
    FlexLayout(Direction direction = Horizontal, int flags=(int)Justify_SpaceEvenly | (int)Align_Center) :
        m_direction(direction),
        m_flags(flags)
    {
    }
    
    void setGap(int value) {m_gap = value;}
    int gap() const {return m_gap;}
    
protected:
    Direction m_direction;
    int m_flags = 0;
    int m_gap = 0;
    
    virtual void update() override
    {
        if (!m_widget)
            return;
        int cnt = items().size();        
        int w = m_widget->width() - marginLeft - marginRight;
        int h = m_widget->height() - marginTop - marginBottom;
        
        if (!cnt || w <= 0 || h <= 0)
            return;
        
        int avg, size;
        int x = marginLeft;
        int y = marginTop;
        int len;
        int sz_min;
        int sz_max;
        int dsz = 0;
        int rem = 0;
        int iter = 0;
        
        if (m_direction == Horizontal)
            size = w;
        else
            size = h;
        
        avg = size / cnt;
        
        std::vector<int> sizes;
        sizes.resize(cnt);
        
        do
        {
            len = m_gap * (cnt - 1);
            int dcnt = 0;
            int i = 0;
            for (Widget *widget: items())
            {
                if (!widget->visible())
                    continue;
                int sz = avg;
                if (m_direction == Horizontal)
                {
                    sz_min = widget->minimumWidth();
                    sz_max = widget->maximumWidth();
                }
                else
                {
                    sz_min = widget->minimumHeight();
                    sz_max = widget->maximumHeight();
                }
                
                if (!dsz && rem > 0 && sz >= sz_min-1 && sz < sz_max)
                {
                    --rem;
                    sz++;
                }
                
                int sz_new = BOUND(sz_min, sz, sz_max);
                if (sz_new == sz)
                    dcnt++;
                len += sz_new;
                sizes[i++] = sz_new;
            }
            if (!dcnt)
            {
                dcnt = 1;
//                break;
            }

            dsz = (size - len) / dcnt;
            if (rem) // ne smoglo(
                dsz++;

            if (size >= len)
                rem = (size - len) % dcnt;
            else
            {
                if (!dsz)
                    --dsz;
                rem = 0;
            }
            avg += dsz;
            iter++;
            if (iter > 100) // fail
                break;
        }
        while (size != len);
        
        int space = size - len;

        int coord = (m_direction == Horizontal)? x: y;
        int jc = m_flags & JustifyContent_Mask;
        int align = m_flags & AlignItems_Mask;
        if (jc == Justify_FlexEnd)
            coord += space;
        else if (jc == Justify_Center)
            coord += space / 2;
        else if (jc == Justify_SpaceBetween && cnt > 1)
            --cnt;
        else if (jc == Justify_SpaceEvenly)
            ++cnt;
        
        int g = 0;
        if (cnt > 0)
            g = space / cnt;
            
        if (jc == Justify_SpaceAround)
            coord += g / 2;
        else if (jc == Justify_SpaceEvenly)
            coord += g;
        
        int i = 0;
        for (Widget *widget: items())
        {
            if (!widget->visible())
                continue;
            int sz = sizes[i++];
            int off2 = 0;
            int size2 = 0;
            int maxSize2 = 0;
            
            if (m_direction == Horizontal)
            {
                size2 = h;
                maxSize2 = widget->maximumHeight();
            }
            else
            {
                size2 = w;
                maxSize2 = widget->maximumWidth();
            }
            
            if (maxSize2 > size2)
                maxSize2 = size2;
            
            if (align == Align_FlexEnd)
                off2 = size2 - maxSize2;
            else if (align == Align_Center)
                off2 = (size2 - maxSize2) / 2;
            
            if (align != Align_Stretch)
                size2 = maxSize2;
            
            if (m_direction == Horizontal)
                setWidgetGeometry(widget, coord, y+off2, sz, size2);
            else
                setWidgetGeometry(widget, x+off2, coord, size2, sz);
            
            g = space / cnt--;
            coord += sz + g + m_gap;
        }
    }

protected:

private:

};
