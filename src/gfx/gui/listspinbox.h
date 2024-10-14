#pragma once

#include "spinbox.h"

class ListSpinBox : public SpinBox
{
public:
    ListSpinBox(Widget *parent=nullptr);

    void addItem(const ByteArray &text);

    void setItems(const ByteArray &list); // items separated with '\n'
    ByteArray items() const; // return list of items separated with '\n'

    int count() const {return m_items.size();}

    const ByteArray &currentText() const;

protected:
    virtual void updateText();

private:
    std::vector<ByteArray> m_items;
};
