#include "gradient.h"

//Gradient::Gradient(Type type) :
//    m_type(type)
//{
//}

void Gradient::addStop(float pos, Color color)
{
    m_stops.push_back(Stop(pos, color));
}
    
Color Gradient::colorAt(float pos)
{
    if (!m_stops.size())
        return Transparent;
    
    if (m_stops.size() == 1)
        return m_stops.front().second;
    
    if (pos <= m_stops.front().first)
        return m_stops.front().second;
    
    if (pos >= m_stops.back().first)
        return m_stops.back().second;
    
    Color col;
    int N = m_stops.size();
    for (int i=1; i<N; i++)
    {
        const Stop &s1 = m_stops[i];
        if (pos < s1.first)
        {
            const Stop &s0 = m_stops[i-1];
            int a = 128;
            if (s1.first != s0.first)
                a = static_cast<int>(255 * (pos - s0.first) / (s1.first - s0.first));
            col = Color::blend(s1.second, s0.second, a);
            break;
        }
    }
    return col;
}

//Color Gradient::colorAt(float x, float y)
//{
//}
