#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

struct KnobSpec
{
    const char* paramId;
    juce::String label;
};

// A stompbox: painted enclosure, a row of knobs, an LED and a footswitch bound to the
// effect's on/off parameter.
class Pedal : public juce::Component
{
public:
    Pedal (juce::AudioProcessorValueTreeState& state, const juce::String& name, const juce::String& subtitle,
           juce::Colour colour, const std::vector<KnobSpec>& knobs, const char* toggleParamId);

    int getNumKnobs() const { return sliders.size(); }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class Footswitch : public juce::ToggleButton
    {
    public:
        void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    };

    juce::Rectangle<float> enclosure() const;
    juce::Colour inkColour() const;

    juce::String name, subtitle;
    juce::Colour colour;

    juce::OwnedArray<juce::Slider> sliders;
    juce::StringArray labels;
    juce::Array<juce::Rectangle<int>> labelAreas;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;

    Footswitch footswitch;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> footswitchAttachment;

    juce::Rectangle<int> nameArea;
    juce::Point<float> ledCentre;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pedal)
};
