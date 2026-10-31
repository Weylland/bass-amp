#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "TapTempo.h"
#include "ToggleSwitch.h"

// Metronome controls: on/off, tempo (knob or TAP), volume, and a screen with the tempo and
// a light per beat of the bar.
class MetronomeUnit : public juce::Component, public juce::SettableTooltipClient, private juce::Timer
{
public:
    explicit MetronomeUnit (BassAmpProcessor&);
    ~MetronomeUnit() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    Metronome& metronome;
    ToggleSwitch onSwitch { "ON" };
    juce::TextButton tapButton { "TAP" };
    juce::Slider tempo { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    juce::Slider volume { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    TapTempo tapTempo;
    int shownBeat = -1;

    juce::Rectangle<int> titleArea, screenArea, tempoLabelArea, volumeLabelArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetronomeUnit)
};
