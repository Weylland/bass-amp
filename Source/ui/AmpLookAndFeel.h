#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Palette
{
    inline const juce::Colour background { 0xff17181c };
    inline const juce::Colour panel      { 0xff22242a };
    inline const juce::Colour track      { 0xff34363d };
    inline const juce::Colour text       { 0xffe8e6e3 };
    inline const juce::Colour textDim    { 0xff8a8d94 };

    inline const juce::Colour amp        { 0xffe0a84f };
    inline const juce::Colour distortion { 0xffe0564f };
    inline const juce::Colour chorus     { 0xff4fc3e0 };
    inline const juce::Colour reverb     { 0xff9b7be0 };
}

class AmpLookAndFeel : public juce::LookAndFeel_V4
{
public:
    AmpLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
