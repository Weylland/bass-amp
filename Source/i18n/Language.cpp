#include "Language.h"

namespace
{
    juce::File settingsFile()
    {
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
            .getChildFile ("Bass Amp").getChildFile ("Language.txt");
    }

    const Language::Info* find (const juce::String& code)
    {
        for (const auto& info : Language::available)
            if (code == info.code)
                return &info;

        return nullptr;
    }

    juce::String& chosen()
    {
        static juce::String code = []
        {
            for (const auto& candidate : { settingsFile().loadFileAsString().trim(),
                                           juce::SystemStats::getUserLanguage().substring (0, 2).toLowerCase() })
                if (find (candidate) != nullptr)
                    return candidate;

            return juce::String ("en");
        }();

        return code;
    }
}

juce::String Language::getCurrent()
{
    return chosen();
}

void Language::setCurrent (const juce::String& code)
{
    if (find (code) == nullptr)
        return;

    chosen() = code;

    if (settingsFile().getParentDirectory().createDirectory())
        settingsFile().replaceWithText (code);
}

const Translations& Language::current()
{
    return find (chosen())->translations();
}

juce::String Language::tr (const char* english)
{
    const auto& table = current().messages;
    const auto found = table.find (english);
    return juce::String::fromUTF8 (found != table.end() ? found->second : english);
}
