#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "ActionButton.h"
#include "LoopPad.h"

// The looper in the practice row: a pad per track, STOP ALL, and OPEN for the full looper.
class LooperUnit : public juce::Component, public juce::SettableTooltipClient, private juce::Timer
{
public:
    explicit LooperUnit (BassAmpProcessor&);
    ~LooperUnit() override;

    std::function<void()> onOpen;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    Looper& looper;
    juce::OwnedArray<LoopPad> pads;
    ActionButton stopAllButton { "STOP ALL" };
    juce::TextButton openButton { "OPEN" };

    juce::Rectangle<int> titleArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LooperUnit)
};
