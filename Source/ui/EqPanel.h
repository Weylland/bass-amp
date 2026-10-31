#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "EqDisplay.h"
#include "ToggleSwitch.h"

// The EQ unit: on/off, Basic (graphic) or Advanced (parametric) mode, and the EQ screen.
class EqPanel : public juce::Component, private juce::Timer
{
public:
    explicit EqPanel (BassAmpProcessor&);
    ~EqPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void setMode (int mode);

    BassAmpProcessor& processor;
    std::atomic<float>* onParam = nullptr;
    std::atomic<float>* modeParam = nullptr;

    ToggleSwitch onSwitch;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> onAttachment;
    juce::TextButton basicButton { "BASIC" }, advancedButton { "ADVANCED" };
    EqDisplay display;
    juce::Rectangle<int> titleArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqPanel)
};
