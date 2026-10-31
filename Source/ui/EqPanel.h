#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "EqDisplay.h"
#include "PresetsButton.h"
#include "ToggleSwitch.h"

// The EQ unit: on/off, Basic (graphic) or Advanced (parametric) mode, the EQ screen, and a
// column of one-touch cuts and band kills.
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
    PresetsButton presetsButton;
    EqDisplay display;
    juce::OwnedArray<ToggleSwitch> cutSwitches;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> cutAttachments;
    juce::Rectangle<int> titleArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqPanel)
};
