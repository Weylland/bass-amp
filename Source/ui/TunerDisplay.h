#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"

// Note name, cents needle and frequency, plus a button that mutes the output while tuning.
class TunerDisplay : public juce::Component, public juce::SettableTooltipClient, private juce::Timer
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
    juce::Rectangle<int> titleArea, screenArea;

    float smoothedMidi = 0.0f;
    bool hasPitch = false;
    int framesWithoutPitch = 0;
};
