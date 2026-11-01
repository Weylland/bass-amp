#pragma once

#include <juce_core/juce_core.h>

// Asks GitHub, in the background, whether a release newer than this build exists. Shared by
// every open editor, so one request per launch; nothing is sent but the request itself.
class UpdateCheck : private juce::Thread
{
public:
    UpdateCheck();
    ~UpdateCheck() override;

    struct Release
    {
        juce::String version, url;
    };

    // Empty until a newer release is found. Any thread.
    Release getNewerRelease() const;

    static bool isNewer (const juce::String& version, const juce::String& than);

private:
    void run() override;

    juce::CriticalSection lock;
    Release newer;
};
