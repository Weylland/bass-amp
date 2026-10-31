#include "Help.h"
#include "../i18n/Language.h"

juce::String Help::get (const juce::String& key)
{
    for (const auto* table : { &Language::current().help, &englishTranslations().help })
        if (const auto found = table->find (key); found != table->end())
            return juce::String::fromUTF8 (found->second);

    return {};
}
