#include "spinbox.h"
#include "core/macros.h"
#include "flexlayout.h"
#include "pushbutton.h"

AbstractSpinBox::AbstractSpinBox(Widget *parent) : Widget(parent)
{
    const FontInfo &fi = font().info();
    setFixedHeight(fi.height() + 4);
    int bw = fi.width('<') * 2;
    PushButton *btnDown = new PushButton("<");
    btnDown->setFixedWidth(bw);
    btnDown->onClick = EVENT(&SpinBox::stepDown);
    PushButton *btnUp = new PushButton(">");
    btnUp->setFixedWidth(bw);
    btnUp->onClick = EVENT(&SpinBox::stepUp);
    m_edit = new LineEdit();
    m_edit->setAlignment(AlignCenter);
    addWidget(btnDown);
    addWidget(m_edit);
    addWidget(btnUp);
    setLayout(new FlexLayout(Horizontal));
}

void AbstractSpinBox::setButtonSymbols(ButtonSymbols bs)
{
    PushButton *btnDown = static_cast<PushButton*>(m_children[0]);
    PushButton *btnUp = static_cast<PushButton*>(m_children[2]);

    if (bs == Arrows)
    {
        btnDown->setText("<");
        btnUp->setText(">");
    }
    else if (bs == PlusMinus)
    {
        btnDown->setText("-");
        btnUp->setText("+");
    }

    btnDown->setVisible(bs != NoButtons);
    btnUp->setVisible(bs != NoButtons);
}

void AbstractSpinBox::stepUp()
{
    if (m_enabled)
        stepBy(1);
}

void AbstractSpinBox::stepDown()
{
    if (m_enabled)
        stepBy(-1);
}

void AbstractSpinBox::setWrapping(bool wrapping)
{
    m_wrapping = wrapping;
}
