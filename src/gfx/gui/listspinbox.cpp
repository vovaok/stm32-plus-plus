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

void ListSpinBox::setItems(const ByteArray &list)
{
    m_items.clear();
    int idx = 0;
    do
    {
        int next_idx = list.indexOf('\n', idx);
        if (next_idx > 0)
            m_items.push_back(list.mid(idx, next_idx - idx));
        else
            m_items.push_back(list.mid(idx));
        idx = next_idx + 1;
    } while (idx > 0);
    setRange(0, m_items.size());
    updateText();
}

ByteArray ListSpinBox::items() const
{
    ByteArray list;
    for (const ByteArray &ba: m_items)
    {
        list.append(ba);
        list.append('\n');
    }
    return list;
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
