#include "TunerDisplay.h"
#include "AmpLookAndFeel.h"

namespace
{
    constexpr int padding = 12;
    constexpr float inTuneCents = 3.0f;

    juce::String noteName (int midiNote)
    {
        static const char* const names[] { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        return juce::String (names[((midiNote % 12) + 12) % 12]) + juce::String (midiNote / 12 - 1);
    }
}

TunerDisplay::TunerDisplay (BassAmpProcessor& processorToUse)
    : processor (processorToUse)
{
    muteButton.setClickingTogglesState (true);
    muteButton.setToggleState (processor.muteOutput.load(), juce::dontSendNotification);
    muteButton.setColour (juce::TextButton::buttonColourId, Palette::panel.brighter (0.08f));
    muteButton.setColour (juce::TextButton::buttonOnColourId, Palette::distortion);
    muteButton.setColour (juce::TextButton::textColourOffId, Palette::textDim);
    muteButton.setColour (juce::TextButton::textColourOnId, Palette::text);
    muteButton.onClick = [this] { processor.muteOutput.store (muteButton.getToggleState()); };
    addAndMakeVisible (muteButton);

    startTimerHz (20);
}

TunerDisplay::~TunerDisplay()
{
    stopTimer();
}

void TunerDisplay::timerCallback()
{
    if (const auto frequency = processor.tuner.detectFrequency())
    {
        const auto midi = 69.0f + 12.0f * std::log2 (*frequency / 440.0f);

        if (! hasPitch || std::abs (midi - smoothedMidi) > 0.5f)
            smoothedMidi = midi;
        else
            smoothedMidi += 0.35f * (midi - smoothedMidi);

        hasPitch = true;
        framesWithoutPitch = 0;
    }
    else if (++framesWithoutPitch > 10)
    {
        hasPitch = false;
    }

    repaint();
}

void TunerDisplay::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (Palette::panel);
    g.fillRoundedRectangle (bounds, 10.0f);
    g.setColour (Palette::chorus.withAlpha (0.35f));
    g.drawRoundedRectangle (bounds, 10.0f, 1.0f);

    auto area = getLocalBounds().reduced (padding);
    g.setColour (Palette::chorus);
    g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    g.drawText ("TUNER", area.removeFromTop (26), juce::Justification::centredLeft);

    const auto nearest = juce::roundToInt (smoothedMidi);
    const auto cents = (smoothedMidi - (float) nearest) * 100.0f;
    const auto inTune = hasPitch && std::abs (cents) <= inTuneCents;
    const auto noteColour = ! hasPitch ? Palette::textDim : (inTune ? juce::Colour (0xff5fd68a) : Palette::text);

    auto meterArea = area.removeFromBottom (34);
    auto textArea = area;

    g.setColour (noteColour);
    g.setFont (juce::FontOptions (44.0f, juce::Font::bold));
    g.drawText (hasPitch ? noteName (nearest) : "--", textArea.removeFromLeft (textArea.getWidth() / 2), juce::Justification::centred);

    g.setColour (Palette::textDim);
    g.setFont (juce::FontOptions (14.0f));

    if (hasPitch)
    {
        const auto frequency = 440.0f * std::pow (2.0f, (smoothedMidi - 69.0f) / 12.0f);
        const auto centsText = (cents >= 0.0f ? "+" : "") + juce::String (juce::roundToInt (cents)) + " cents";
        g.drawText (juce::String (frequency, 1) + " Hz", textArea.removeFromTop (textArea.getHeight() / 2), juce::Justification::centredBottom);
        g.drawText (centsText, textArea, juce::Justification::centredTop);
    }

    const auto scale = meterArea.toFloat().reduced (4.0f, 10.0f);
    g.setColour (Palette::track);
    g.fillRoundedRectangle (scale, 3.0f);

    for (int tick = -50; tick <= 50; tick += 10)
    {
        const auto x = scale.getX() + scale.getWidth() * ((float) tick + 50.0f) / 100.0f;
        g.setColour (tick == 0 ? Palette::text : Palette::textDim.withAlpha (0.5f));
        g.fillRect (x - 0.5f, scale.getY() - (tick == 0 ? 6.0f : 3.0f), 1.0f, scale.getHeight() + (tick == 0 ? 12.0f : 6.0f));
    }

    if (hasPitch)
    {
        const auto x = scale.getX() + scale.getWidth() * (juce::jlimit (-50.0f, 50.0f, cents) + 50.0f) / 100.0f;
        g.setColour (inTune ? juce::Colour (0xff5fd68a) : Palette::distortion);
        g.fillRoundedRectangle (juce::Rectangle<float> (6.0f, scale.getHeight() + 14.0f).withCentre ({ x, scale.getCentreY() }), 3.0f);
    }
}

void TunerDisplay::resized()
{
    auto header = getLocalBounds().reduced (padding).removeFromTop (26);
    muteButton.setBounds (header.removeFromRight (64).withSizeKeepingCentre (64, 22));
}
