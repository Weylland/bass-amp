#include "Pedal.h"
#include "AmpLookAndFeel.h"

namespace
{
    constexpr float cornerSize = 12.0f;
    constexpr int padding = 12;
    constexpr int labelHeight = 13;
    constexpr int maxKnobSize = 48;
    constexpr int footswitchSize = 42;
}

Pedal::Pedal (juce::AudioProcessorValueTreeState& state, const juce::String& pedalName, const juce::String& pedalSubtitle,
              juce::Colour pedalColour, const std::vector<KnobSpec>& knobs, const char* toggleParamId)
    : name (pedalName), subtitle (pedalSubtitle), colour (pedalColour)
{
    for (const auto& spec : knobs)
    {
        auto* slider = sliders.add (new juce::Slider (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox));
        slider->getProperties().set (KnobStyle::property, KnobStyle::pedal);
        slider->setPopupDisplayEnabled (true, true, nullptr);
        addAndMakeVisible (slider);

        attachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (state, spec.paramId, *slider));

        if (auto* parameter = state.getParameter (spec.paramId))
            slider->setDoubleClickReturnValue (true, parameter->convertFrom0to1 (parameter->getDefaultValue()));

        labels.add (spec.label.toUpperCase());
    }

    footswitch.onStateChange = [this] { repaint(); };
    addAndMakeVisible (footswitch);
    footswitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (state, toggleParamId, footswitch);
}

juce::Rectangle<float> Pedal::enclosure() const
{
    return getLocalBounds().toFloat().reduced (3.0f, 2.0f).withTrimmedBottom (4.0f);
}

juce::Colour Pedal::inkColour() const
{
    return colour.getPerceivedBrightness() > 0.62f ? juce::Colour (0xff141414) : juce::Colour (0xfff6f3ee);
}

void Pedal::paint (juce::Graphics& g)
{
    const auto body = enclosure();

    juce::Path outline;
    outline.addRoundedRectangle (body, cornerSize);
    juce::DropShadow (juce::Colours::black.withAlpha (0.7f), 14, { 0, 6 }).drawForPath (g, outline);

    g.setGradientFill (juce::ColourGradient (colour.brighter (0.28f), body.getX(), body.getY(),
                                             colour.darker (0.45f), body.getX(), body.getBottom(), false));
    g.fillPath (outline);

    g.saveState();
    g.reduceClipRegion (outline);
    g.setGradientFill (juce::ColourGradient (juce::Colours::white.withAlpha (0.22f), body.getX(), body.getY(),
                                             juce::Colours::transparentWhite, body.getX() + body.getWidth() * 0.55f,
                                             body.getY() + body.getHeight() * 0.45f, false));
    g.fillRect (body);

    // The lower deck where the footswitch sits.
    const auto deck = body.withTop ((float) footswitch.getY() - 12.0f);
    g.setColour (juce::Colours::black.withAlpha (0.18f));
    g.fillRect (deck);
    g.setColour (juce::Colours::white.withAlpha (0.12f));
    g.fillRect (deck.withHeight (1.0f));
    g.restoreState();

    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.strokePath (outline, juce::PathStrokeType (1.5f));
    g.setColour (juce::Colours::white.withAlpha (0.18f));
    g.drawRoundedRectangle (body.reduced (1.5f), cornerSize - 1.5f, 1.0f);

    const auto ink = inkColour();
    g.setColour (ink.withAlpha (0.85f));
    g.setFont (Fonts::label (10.5f));

    for (int i = 0; i < labels.size(); ++i)
        g.drawFittedText (labels[i], labelAreas[i], juce::Justification::centred, 1, 0.7f);

    g.setColour (ink);
    g.setFont (Fonts::display (24.0f).withKerningFactor (0.04f));
    g.drawText (name, nameArea.withHeight (26), juce::Justification::centred);
    g.setColour (ink.withAlpha (0.6f));
    g.setFont (Fonts::label (10.0f));
    g.drawText (subtitle.toUpperCase(), nameArea.withTrimmedTop (26), juce::Justification::centredTop);

    Draw::led (g, ledCentre, 4.5f, Palette::ledRed, footswitch.getToggleState());
}

void Pedal::resized()
{
    const auto body = enclosure().toNearestInt();
    auto area = body.reduced (padding);

    footswitch.setBounds (juce::Rectangle<int> (footswitchSize, footswitchSize)
                              .withCentre ({ body.getCentreX(), area.getBottom() - footswitchSize / 2 }));
    area.setBottom (footswitch.getY() - 14);

    nameArea = area.removeFromBottom (40);
    ledCentre ={ (float) body.getRight() - 22.0f, (float) footswitch.getBounds().getCentreY() };

    const auto cellWidth = area.getWidth() / juce::jmax (1, sliders.size());
    const auto knobSize = juce::jmin (maxKnobSize, cellWidth - 4, area.getHeight() - labelHeight - 2);
    labelAreas.clearQuick();

    for (auto* slider : sliders)
    {
        auto cell = area.removeFromLeft (cellWidth);
        labelAreas.add (cell.removeFromTop (labelHeight));
        slider->setBounds (cell.withSizeKeepingCentre (knobSize + 8, knobSize + 8));
    }
}

void Pedal::Footswitch::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
    const auto centre = bounds.getCentre();
    const auto radius = bounds.getWidth() / 2.0f;

    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillEllipse (bounds.translated (0.0f, 2.5f));

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff9a9ea6), centre.x, centre.y - radius,
                                             juce::Colour (0xff2b2d32), centre.x, centre.y + radius, false));
    g.fillEllipse (bounds);

    const auto press = shouldDrawButtonAsDown ? 1.5f : 0.0f;
    const auto dome = bounds.reduced (radius * 0.2f).translated (0.0f, press);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff7f8fa), dome.getX() + dome.getWidth() * 0.3f, dome.getY(),
                                             juce::Colour (0xff70747c), dome.getRight(), dome.getBottom(), true));
    g.fillEllipse (dome);
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.drawEllipse (dome, 1.0f);

    for (auto ring = 0.45f; ring < 0.95f; ring += 0.25f)
    {
        g.setColour (juce::Colours::black.withAlpha (0.12f));
        g.drawEllipse (dome.withSizeKeepingCentre (dome.getWidth() * ring, dome.getHeight() * ring), 1.0f);
    }

    g.setColour (juce::Colours::white.withAlpha (shouldDrawButtonAsHighlighted ? 0.8f : 0.6f));
    g.fillEllipse (dome.withSizeKeepingCentre (dome.getWidth() * 0.25f, dome.getHeight() * 0.16f)
                       .translated (-dome.getWidth() * 0.12f, -dome.getHeight() * 0.22f));
}
