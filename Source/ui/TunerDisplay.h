#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"

// Note name, cents needle and frequency, plus a button that mutes the output while tuning.
class TunerDisplay : public juce::Component, private juce::Timer
{
public:
    explicit TunerDisplay (BassAmpProcessor&);
    ~TunerDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    BassAmpProcessor& processor;
    juce::TextButton muteButton { "MUTE" };

    float smoothedMidi = 0.0f;
    bool hasPitch = false;
    int framesWithoutPitch = 0;
};
