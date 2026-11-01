#include "UpdateCheck.h"

namespace
{
    constexpr auto latestReleaseUrl = "https://api.github.com/repos/Weylland/bass-amp/releases/latest";
    constexpr int timeoutMs = 4000;
}

UpdateCheck::UpdateCheck() : juce::Thread ("Update check")
{
    startThread (juce::Thread::Priority::low);
}

UpdateCheck::~UpdateCheck()
{
    stopThread (timeoutMs + 1000);
}

UpdateCheck::Release UpdateCheck::getNewerRelease() const
{
    const juce::ScopedLock scopedLock (lock);
    return newer;
}

bool UpdateCheck::isNewer (const juce::String& version, const juce::String& than)
{
    const auto parts = juce::StringArray::fromTokens (version, ".", {});
    const auto current = juce::StringArray::fromTokens (than, ".", {});

    for (int i = 0; i < 3; ++i)
        if (const auto difference = parts[i].getIntValue() - current[i].getIntValue(); difference != 0)
            return difference > 0;

    return false;
}

void UpdateCheck::run()
{
    const auto options = juce::URL::InputStreamOptions (juce::URL::ParameterHandling::inAddress)
                             .withConnectionTimeoutMs (timeoutMs)
                             .withExtraHeaders ("User-Agent: Bass-Amp\r\nAccept: application/vnd.github+json");
    const auto stream = juce::URL (latestReleaseUrl).createInputStream (options);

    if (stream == nullptr || threadShouldExit())
        return;

    const auto release = juce::JSON::parse (stream->readEntireStreamAsString());
    const auto version = release["tag_name"].toString().trimCharactersAtStart ("v");

    if (isNewer (version, JucePlugin_VersionString))
    {
        const juce::ScopedLock scopedLock (lock);
        newer = { version, release["html_url"].toString() };
    }
}
