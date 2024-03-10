#include "translator.h"
#include "label.h"
#include "pushbutton.h"

Translator::TranslationItem &Translator::create(SetTextEvent e, const ByteArray &defaultText)
{
    TranslationItem *item = new TranslationItem(e, defaultText);
    m_items.push_back(item);
    return *item;
}

Translator::TranslationItem &Translator::create(Label *label)
{
    return create(CLOSURE(label, &Label::setText), label->text());
}

Translator::TranslationItem &Translator::create(PushButton *pushButton)
{
    return create(CLOSURE(pushButton, &PushButton::setText), pushButton->text());
}

void Translator::switchLanguage(Language lang)
{
    m_lang = lang;
    for (TranslationItem *item: m_items)
    {
        item->translate(lang);
    }
}
