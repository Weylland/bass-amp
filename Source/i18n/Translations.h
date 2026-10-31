#pragma once

#include <juce_core/juce_core.h>

#include <map>

// Everything the interface says in one language: the help bubbles by help key, and the
// messages and menus by their English text. English has no message table: its text is
// the key itself.
struct Translations
{
    std::map<juce::String, const char*> help;
    std::map<juce::String, const char*> messages;
};

const Translations& englishTranslations();
const Translations& frenchTranslations();
const Translations& spanishTranslations();
const Translations& germanTranslations();
const Translations& italianTranslations();
const Translations& portugueseTranslations();
