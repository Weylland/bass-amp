#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// The button that opens a module's preset menu: the three lines every pedal shows, then PRESETS.
class PresetsButton : public juce::TextButton
{
public:
    PresetsButton() : juce::TextButton ("PRESETS") {}

    static void drawIcon (juce::Graphics& g, juce::Rectangle<float> area)
    {
        for (auto row = 0; row < 3; ++row)
        {
            const auto y = area.getY() + area.getHeight() * (float) row / 2.0f;
            g.drawLine (area.getX(), y, area.getRight(), y, 1.4f);
        }
    }

    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto& theme = getLookAndFeel();
        theme.drawButtonBackground (g, *this, findColour (buttonColourId), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        const auto font = theme.getTextButtonFont (*this, getHeight());
        const auto textWidth = juce::GlyphArrangement::getStringWidth (font, getButtonText());
        auto content = getLocalBounds().toFloat().withSizeKeepingCentre (textWidth + 16.0f, (float) getHeight());

        g.setColour (findColour (textColourOffId));
        drawIcon (g, content.removeFromLeft (10.0f).withSizeKeepingCentre (10.0f, 8.0f));
        g.setFont (font);
        g.drawText (getButtonText(), content, juce::Justification::centredRight);
    }
};
