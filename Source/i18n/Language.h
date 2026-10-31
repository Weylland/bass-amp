#pragma once

#include <juce_core/juce_core.h>

#include "Translations.h"

#include <array>

// The interface language: help bubbles, messages and menus. Control labels stay in English,
// as printed on real gear. The choice is saved for every session; without one, the system
// language is used when it is available, else English. Message thread only.
namespace Language
{
    struct Info
    {
        const char* code;
        const char* name;   // in the language itself, UTF-8
        const Translations& (*translations)();
    };

    inline const std::array<Info, 6> available {{
        { "en", "English", englishTranslations },
        { "fr", "Fran\xc3\xa7" "ais", frenchTranslations },
        { "es", "Espa\xc3\xb1" "ol", spanishTranslations },
        { "de", "Deutsch", germanTranslations },
        { "it", "Italiano", italianTranslations },
        { "pt", "Portugu\xc3\xaa" "s", portugueseTranslations },
    }};

    juce::String getCurrent();
    void setCurrent (const juce::String& code);
    const Translations& current();

    // The interface text for this English text, in the current language.
    juce::String tr (const char* english);

    // Hands the messages to JUCE, which then translates its own windows (the audio settings).
    void applyToJuce();
}
