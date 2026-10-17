#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

struct KnobSpec
{
    const char* paramId;
    juce::String label;
};

// A titled panel of rotary knobs, with an optional ON/OFF toggle bound to a bool parameter.
class Section : public juce::Component
{
public:
    Section (juce::AudioProcessorValueTreeState& state, const juce::String& title, juce::Colour accent,
             const std::vector<KnobSpec>& knobs, const char* toggleParamId = nullptr);

    int getNumKnobs() const { return sliders.size(); }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void refreshEnabledLook();

    juce::String title;
    juce::Colour accent;

    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> labels;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;

    std::unique_ptr<juce::ToggleButton> toggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toggleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Section)
};
