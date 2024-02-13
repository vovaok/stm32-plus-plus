#pragma once

#include "spinbox.h"

class ListSpinBox : public SpinBox
{
public:
    ListSpinBox(Widget *parent=nullptr);
    
    void addItem(const ByteArray &text);
    
    int count() const {return m_items.size();}
    
    const ByteArray &currentText() const;
    
protected:
    virtual void updateText();
    
private:
    std::vector<ByteArray> m_items;
};
