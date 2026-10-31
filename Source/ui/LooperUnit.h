#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"

// Looper controls: the main button records, plays and overdubs in turn; STOP, CLEAR, the
// loop level, and a screen with the state and the loop position.
class LooperUnit : public juce::Component, public juce::SettableTooltipClient, private juce::Timer
{
public:
    explicit LooperUnit (BassAmpProcessor&);
    ~LooperUnit() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    Looper& looper;
    juce::TextButton mainButton { "REC" }, stopButton { "STOP" }, clearButton { "CLEAR" };
    juce::Slider level { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    Looper::State shownState = Looper::State::empty;

    juce::Rectangle<int> titleArea, screenArea, levelLabelArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LooperUnit)
};
