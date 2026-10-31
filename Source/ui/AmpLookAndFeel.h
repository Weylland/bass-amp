#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Palette
{
    inline const juce::Colour background { 0xff0f1013 };
    inline const juce::Colour panel      { 0xff1c1d22 };
    inline const juce::Colour track      { 0xff34363d };
    inline const juce::Colour text       { 0xffece8e1 };
    inline const juce::Colour textDim    { 0xff8a8d94 };
    inline const juce::Colour gold       { 0xffe8b75a };
    inline const juce::Colour screen     { 0xff0a0d12 };
    inline const juce::Colour ledGreen   { 0xff52e08a };
    inline const juce::Colour ledYellow  { 0xffffc94a };
    inline const juce::Colour ledRed     { 0xffff4646 };

    inline const juce::Colour gate       { 0xff3f8f5a };
    inline const juce::Colour compressor { 0xff2f6fc0 };
    inline const juce::Colour octaver    { 0xff7b3fb8 };
    inline const juce::Colour synth      { 0xffc8357e };
    inline const juce::Colour pitch      { 0xff7fae2c };
    inline const juce::Colour filter     { 0xffe2c03c };
    inline const juce::Colour envFilter  { 0xff1fa58a };
    inline const juce::Colour wah        { 0xff3a3d45 };
    inline const juce::Colour fuzz       { 0xffd2652a };
    inline const juce::Colour distortion { 0xffc23434 };
    inline const juce::Colour phaser     { 0xffe08a1f };
    inline const juce::Colour flanger    { 0xffa8325a };
    inline const juce::Colour chorus     { 0xff2aa6c8 };
    inline const juce::Colour tremolo    { 0xff8c6a3c };
    inline const juce::Colour delay      { 0xff3b4fb8 };
    inline const juce::Colour reverb     { 0xff9a9ea8 };
}

// Font helpers: a condensed display face for titles and silkscreen labels.
namespace Fonts
{
    juce::FontOptions display (float height, bool bold = true);
    juce::FontOptions label (float height);
}

// Knob looks, picked per slider with the "knobStyle" property.
namespace KnobStyle
{
    inline const juce::Identifier property { "knobStyle" };
    inline constexpr auto pedal   = "pedal";     // small black stompbox knob
    inline constexpr auto skirted = "skirted";   // big knurled skirt with a metal cap
    inline constexpr auto chicken = "chicken";   // vintage chicken-head pointer
    inline constexpr auto modern  = "modern";    // flat disc with a value arc
}

// Toggle looks, picked per button with the "toggleStyle" property. The default is an LED pill.
namespace ToggleStyle
{
    inline const juce::Identifier property { "toggleStyle" };
    inline constexpr auto batSwitch = "bat";     // chrome toggle switch on an amp panel
}

class AmpLookAndFeel : public juce::LookAndFeel_V4
{
public:
    AmpLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                           float maxSliderPos, juce::Slider::SliderStyle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                       int buttonW, int buttonH, juce::ComboBox&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;

    void drawBubble (juce::Graphics&, juce::BubbleComponent&, const juce::Point<float>& tip,
                     const juce::Rectangle<float>& body) override;
    juce::Font getSliderPopupFont (juce::Slider&) override;
    int getSliderPopupPlacement (juce::Slider&) override;

    void drawPopupMenuBackground (juce::Graphics&, int width, int height) override;

    juce::Rectangle<int> getTooltipBounds (const juce::String& tipText, juce::Point<int> screenPos,
                                           juce::Rectangle<int> parentArea) override;
    void drawTooltip (juce::Graphics&, const juce::String& text, int width, int height) override;

private:
    void drawPedalKnob (juce::Graphics&, juce::Rectangle<float>, float angle, juce::Slider&);
    void drawSkirtedKnob (juce::Graphics&, juce::Rectangle<float>, float angle, juce::Slider&);
    void drawChickenKnob (juce::Graphics&, juce::Rectangle<float>, float angle, juce::Slider&);
    void drawModernKnob (juce::Graphics&, juce::Rectangle<float>, float angle, float startAngle, float endAngle,
                         float sliderPos, juce::Slider&);
    void drawBatSwitch (juce::Graphics&, juce::ToggleButton&);
};

// Shared drawing helpers.
namespace Draw
{
    // An LED with a glow halo when lit.
    void led (juce::Graphics&, juce::Point<float> centre, float radius, juce::Colour colour, bool lit);

    // A recessed screen: dark glass with an inner shadow and a soft reflection.
    void screen (juce::Graphics&, juce::Rectangle<float>, float cornerSize);

    // Phillips-head screw.
    void screw (juce::Graphics&, juce::Point<float> centre, float radius);

    // Fine horizontal grain, for brushed metal.
    void brushedMetal (juce::Graphics&, juce::Rectangle<float>, juce::Colour base, float cornerSize);

    // A dark brushed-metal unit with a drop shadow and corner screws. Returns its body.
    juce::Rectangle<float> unitChassis (juce::Graphics&, juce::Rectangle<float> bounds);
}
