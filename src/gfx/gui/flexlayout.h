#pragma once

#include "layout.h"

class FlexLayout : public Layout
{
public:  
    enum JustifyContent
    {
        FlexStart   = 0x0001,
        FlexEnd     = 0x0002,
        Center      = 0x0003,
        SpaceBetween= 0x0004, 
        SpaceAround = 0x0005,
        SpaceEvenly = 0x0006,
        JustifyContent_Mask = 0x0007
    };
  
    FlexLayout(Direction direction = Horizontal, int flags=SpaceEvenly) :
        m_direction(direction),
        m_flags(flags)
    {
    }
    
protected:
    Direction m_direction;
    int m_flags;
    
    virtual void update() override
    {
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
            len = 0;
            int dcnt = 0;
            int i = 0;
            for (Widget *widget: items())
            {
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

        int coord = x;
        int jc = m_flags & JustifyContent_Mask;
        if (jc == FlexEnd)
            coord += space;
        else if (jc == Center)
            coord += space / 2;
        else if (jc == SpaceBetween && cnt > 1)
            --cnt;
        else if (jc == SpaceEvenly)
            ++cnt;
        
        int g = 0;
        if (cnt > 0)
            g = space / cnt;
            
        if (jc == SpaceAround)
            coord += g / 2;
        else if (jc == SpaceEvenly)
            coord += g;
        
        int i = 0;
        for (Widget *widget: items())
        {
            int sz = sizes[i++];
            
            if (m_direction == Horizontal)
                setWidgetGeometry(widget, coord, y, sz, h);
            else
                setWidgetGeometry(widget, x, coord, w, sz);
            
            g = space / cnt--;
            coord += sz + g;
        }
    }

protected:

private:

};