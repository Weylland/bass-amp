#include "Section.h"
#include "AmpLookAndFeel.h"

namespace
{
    constexpr int padding = 12;
    constexpr int headerHeight = 26;
    constexpr int labelHeight = 18;
}

Section::Section (juce::AudioProcessorValueTreeState& state, const juce::String& sectionTitle, juce::Colour sectionAccent,
                  const std::vector<KnobSpec>& knobs, const char* toggleParamId,
                  const std::vector<const char*>& choiceParamIds)
    : title (sectionTitle), accent (sectionAccent)
{
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    for (const auto& spec : knobs)
    {
        auto* slider = sliders.add (new juce::Slider (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow));
        slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 16);
        slider->setColour (juce::Slider::rotarySliderFillColourId, accent);
        addAndMakeVisible (slider);

        sliderAttachments.add (new Attachment (state, spec.paramId, *slider));

        if (auto* parameter = state.getParameter (spec.paramId))
            slider->setDoubleClickReturnValue (true, parameter->convertFrom0to1 (parameter->getDefaultValue()));

        auto* label = labels.add (new juce::Label ({}, spec.label));
        label->setJustificationType (juce::Justification::centred);
        label->setFont (juce::FontOptions (12.0f));
        addAndMakeVisible (label);
    }

    for (const auto* paramId : choiceParamIds)
    {
        auto* combo = choices.add (new juce::ComboBox());

        if (auto* parameter = dynamic_cast<juce::AudioParameterChoice*> (state.getParameter (paramId)))
            combo->addItemList (parameter->choices, 1);

        addAndMakeVisible (combo);
        choiceAttachments.add (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (state, paramId, *combo));
    }

    if (toggleParamId != nullptr)
    {
        toggle = std::make_unique<juce::ToggleButton>();
        toggle->setColour (juce::ToggleButton::tickColourId, accent);
        toggle->onStateChange = [this] { refreshEnabledLook(); };
        addAndMakeVisible (*toggle);

        toggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (state, toggleParamId, *toggle);
    }

    refreshEnabledLook();
}

void Section::refreshEnabledLook()
{
    const auto alpha = (toggle == nullptr || toggle->getToggleState()) ? 1.0f : 0.45f;

    for (auto* slider : sliders)
        slider->setAlpha (alpha);

    for (auto* label : labels)
        label->setAlpha (alpha);
}

void Section::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat().reduced (1.0f);

    g.setColour (Palette::panel);
    g.fillRoundedRectangle (bounds, 10.0f);
    g.setColour (accent.withAlpha (0.35f));
    g.drawRoundedRectangle (bounds, 10.0f, 1.0f);

    g.setColour (accent);
    g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    g.drawText (title, getLocalBounds().reduced (padding).removeFromTop (headerHeight), juce::Justification::centredLeft);
}

void Section::resized()
{
    auto area = getLocalBounds().reduced (padding);
    auto header = area.removeFromTop (headerHeight);

    if (toggle != nullptr)
        toggle->setBounds (header.removeFromRight (64).withSizeKeepingCentre (64, 22));

    for (int i = choices.size(); --i >= 0;)
    {
        choices[i]->setBounds (header.removeFromRight (110).withSizeKeepingCentre (110, 24));
        header.removeFromRight (8);
    }

    area.removeFromTop (6);

    const auto cellWidth = area.getWidth() / juce::jmax (1, sliders.size());

    for (int i = 0; i < sliders.size(); ++i)
    {
        auto cell = area.removeFromLeft (cellWidth);
        labels[i]->setBounds (cell.removeFromTop (labelHeight));
        sliders[i]->setBounds (cell);
    }
}
