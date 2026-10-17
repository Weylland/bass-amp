#include "AmpLookAndFeel.h"

AmpLookAndFeel::AmpLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, Palette::textDim);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId, Palette::track);
    setColour (juce::Label::textColourId, Palette::text);
    setColour (juce::ResizableWindow::backgroundColourId, Palette::background);
}

void AmpLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (4.0f);
    const auto centre = bounds.getCentre();
    const auto arcRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f - 3.0f;
    const auto knobRadius = arcRadius - 8.0f;

    if (knobRadius <= 0.0f)
        return;

    const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto isBipolar = slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0;
    const auto originAngle = isBipolar
        ? rotaryStartAngle + (float) slider.valueToProportionOfLength (0.0) * (rotaryEndAngle - rotaryStartAngle)
        : rotaryStartAngle;

    const juce::PathStrokeType stroke (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

    juce::Path track;
    track.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (Palette::track);
    g.strokePath (track, stroke);

    juce::Path value;
    value.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                         juce::jmin (originAngle, angle), juce::jmax (originAngle, angle), true);
    g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId));
    g.strokePath (value, stroke);

    const auto knob = juce::Rectangle<float> (knobRadius * 2.0f, knobRadius * 2.0f).withCentre (centre);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3a3d45), centre.x, knob.getY(),
                                             juce::Colour (0xff1c1d22), centre.x, knob.getBottom(), false));
    g.fillEllipse (knob);
    g.setColour (juce::Colour (0xff0e0f12));
    g.drawEllipse (knob, 1.5f);

    juce::Path pointer;
    pointer.addRoundedRectangle (-1.5f, -knobRadius + 4.0f, 3.0f, knobRadius * 0.45f, 1.5f);
    g.setColour (Palette::text);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre));
}

void AmpLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool, bool)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    const auto radius = bounds.getHeight() / 2.0f;
    const auto isOn = button.getToggleState();
    const auto accent = button.findColour (juce::ToggleButton::tickColourId);

    g.setColour (isOn ? accent.withAlpha (0.18f) : Palette::panel.brighter (0.08f));
    g.fillRoundedRectangle (bounds, radius);
    g.setColour (isOn ? accent : Palette::track.brighter (0.2f));
    g.drawRoundedRectangle (bounds, radius, 1.0f);

    const auto led = juce::Rectangle<float> (8.0f, 8.0f).withCentre ({ bounds.getX() + radius + 2.0f, bounds.getCentreY() });
    g.setColour (isOn ? accent : Palette::textDim.darker (0.4f));
    g.fillEllipse (led);

    g.setColour (isOn ? Palette::text : Palette::textDim);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText (isOn ? "ON" : "OFF", bounds.withTrimmedLeft (radius + 6.0f), juce::Justification::centred);
}
