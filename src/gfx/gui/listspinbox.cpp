#include "listspinbox.h"

ListSpinBox::ListSpinBox(Widget *parent) : SpinBox(parent)
{
    setRange(0, 0);
}

void ListSpinBox::addItem(const ByteArray &text)
{
    m_items.push_back(text);
    setRange(0, m_items.size() - 1);
    updateText();
}

const ByteArray &ListSpinBox::currentText() const
{
    return lineEdit()->text();
}

void ListSpinBox::updateText()
{
    if (value() < count())
    {
        lineEdit()->setText(m_items[value()]);
    }
}
