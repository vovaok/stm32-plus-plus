#pragma once

#include "layout.h"

class FlexLayout : public Layout
{
public:    
    FlexLayout(Direction direction = Horizontal) :
        m_direction(direction)
    {
    }
    
protected:
    Direction m_direction;
    
    virtual void update() override
    {
        int cnt = items().size();        
        int w = m_widget->width();
        int h = m_widget->height();
        
        if (!cnt || !w || !h)
            return;
        
        int avg, size;
        int x = 0;
        int y = 0;
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
        
        int gap = 0, gaprem = 0;
        if (cnt > 1)
        {
            gap = (size - len) / (cnt - 1);
            gaprem = (size - len) % (cnt - 1);
        }
        
        int coord = 0;
        int i = 0;
        for (Widget *widget: items())
        {
            int sz = sizes[i++];
            
            if (m_direction == Horizontal)
                setWidgetGeometry(widget, coord, y, sz, h);
            else
                setWidgetGeometry(widget, x, coord, w, sz);
            
            coord += sz + gap;
            if (gaprem > 0)
            {
                coord++;
                --gaprem;
            }
            else if (gaprem < 0)
            {
                --coord;
                gaprem++;
            }
        }
    }

protected:

private:

};