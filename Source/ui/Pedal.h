#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "../PedalChain.h"
#include "TapTempo.h"
#include "ToggleSwitch.h"

struct KnobSpec
{
    const char* paramId;
    juce::String label;
};

// A stompbox: painted enclosure, a row of knobs, an LED and a footswitch bound to the
// effect's on/off parameter. Knobs on choice parameters click between positions and show
// the current choice as their label.
class Pedal : public juce::Component
{
public:
    Pedal (juce::AudioProcessorValueTreeState& state, PedalId id, const juce::String& name, const juce::String& subtitle,
           juce::Colour colour, const std::vector<KnobSpec>& knobs, const char* toggleParamId);

    PedalId getId() const { return id; }
    int getNumKnobs() const { return sliders.size(); }

    // A TAP button on the deck: the average of the last taps sets this time parameter, in ms.
    void addTapTempo (const char* timeParamId);

    std::function<void()> onRemove;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class Footswitch : public ToggleSwitch
    {
    public:
        void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    };

    class RemoveButton : public juce::Button
    {
    public:
        RemoveButton() : juce::Button ("Remove") { setTooltip ("Remove this pedal"); }
        void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    };

    juce::Rectangle<float> enclosure() const;
    juce::Colour inkColour() const;
    juce::String labelFor (int knob) const;

    juce::AudioProcessorValueTreeState& state;
    PedalId id;
    juce::String name, subtitle;
    juce::Colour colour;

    juce::OwnedArray<juce::Slider> sliders;
    juce::StringArray labels;
    std::vector<juce::AudioParameterChoice*> choices;
    juce::Array<juce::Rectangle<int>> labelAreas;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;

    Footswitch footswitch;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> footswitchAttachment;
    RemoveButton removeButton;

    std::unique_ptr<juce::TextButton> tapButton;
    juce::RangedAudioParameter* tapParameter = nullptr;
    TapTempo tapTempo;

    juce::Rectangle<int> nameArea;
    juce::Point<float> ledCentre;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pedal)
};
