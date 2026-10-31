#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "ToggleSwitch.h"

// Backing track player: load a file, play or pause, stop, seek, loop, volume, and RE-AMP,
// which runs the file through the rig instead of mixing it into the output.
class PlayerUnit : public juce::Component, private juce::Timer
{
public:
    explicit PlayerUnit (BassAmpProcessor&);
    ~PlayerUnit() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void chooseFile();

    BackingTrack& track;
    juce::TextButton loadButton { "LOAD" }, playButton { "PLAY" }, stopButton { "STOP" };
    ToggleSwitch loopSwitch { "LOOP" }, reampSwitch { "RE-AMP" };
    juce::Slider position { juce::Slider::LinearHorizontal, juce::Slider::NoTextBox };
    juce::Slider volume { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::Rectangle<int> titleArea, screenArea, volumeLabelArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerUnit)
};
