#pragma once

#include "guitypes.h"
#include "core/coretypes.h"
#include <vector>
#include <map>

class Label;
class PushButton;

class Translator
{
public:
    typedef Closure<void(const ByteArray &)> SetTextEvent;

    class TranslationItem
    {
    public:
        TranslationItem &add(Language lang, const ByteArray &text)
        {
            m_text[lang] = text;
            return *this;
        }
        TranslationItem &ru(const ByteArray &text) {return add(ru_RU, text);}
        TranslationItem &en(const ByteArray &text) {return add(en_US, text);}

        const ByteArray &text(Language lang) const
        {
            if (m_text.count(lang))
                return m_text.at(lang);
            return m_text.at(AnyLanguage);
        }
        const ByteArray &ru() const {return text(ru_RU);}
        const ByteArray &en() const {return text(en_US);}

    private:
        TranslationItem(SetTextEvent setter, const ByteArray &defaultText = ByteArray()) :
            m_setter(setter)
        {
            m_text[AnyLanguage] = defaultText;
        }
        SetTextEvent m_setter;
        std::map<Language, ByteArray> m_text;
        void translate(Language lang) {m_setter(text(lang));}
        friend class Translator;
    };

    TranslationItem &create(SetTextEvent e, const ByteArray &defaultText);
    TranslationItem &create(Label *label);
    TranslationItem &create(PushButton *pushButton);

    void switchLanguage(Language lang);

private:
    std::vector<TranslationItem *> m_items;
};
