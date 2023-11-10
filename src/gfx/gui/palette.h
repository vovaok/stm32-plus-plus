#pragma once

#include "../color.h"

class Palette
{
public:
    enum ColorRole
    {
        Window,
        WindowText,
        Button,
        ButtonText,
        Base,
        Text,
        Light,
        Dark,
        Shadow,
        Accent,
        Disabled,
        DisabledText
    };

    Color color(ColorRole role)
    {
        return m_colors[role];
    }

    inline Color window() const {return m_colors[Window];}
    inline Color windowText() const {return m_colors[WindowText];}
    inline Color button() const {return m_colors[Button];}
    inline Color buttonText() const {return m_colors[ButtonText];}
    inline Color base() const {return m_colors[Base];}
    inline Color text() const {return m_colors[Text];}
    inline Color light() const {return m_colors[Light];}
    inline Color dark() const {return m_colors[Dark];}
    inline Color shadow() const {return m_colors[Shadow];}
    inline Color accent() const {return m_colors[Accent];}
    inline Color disabled() const {return m_colors[Disabled];}
    inline Color disabledText() const {return m_colors[DisabledText];}

private:
    Color m_colors[12] =
    {
        Color(224, 224, 224),   // Window
        Black,                  // WindowText
        Color(192, 192, 192),   // Button
        Black,                  // ButtonText
        White,                  // Base
        Black,                  // Text
        Color(240, 240, 240),   // Light
        Gray,                   // Dark
        Color(64, 64, 64),      // Shadow
        Orange,                 // Accent
        Color(224, 224, 224),   // Disabled
        Gray                    // DisabledText
    };
};
