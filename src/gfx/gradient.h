#pragma once

#include "color.h"
#include <vector>

class Gradient
{
public:
//    enum Type
//    {
//        NoGradient = 0,
//        Linear,
//        Radial,
//        Conical
//    };
    
    typedef std::pair<float, Color> Stop;
    
//    Gradient(Type type = NoGradient);
    
    // now one must add stops strongly in ascending order
    //! @todo implement sorting
    void addStop(float pos, Color color);
    
    Color colorAt(float pos); // 1D representation
//    Color colorAt(float x, float y); // 2D representation
    
private:
//    Type m_type = NoGradient;
    std::vector<Stop> m_stops;
};