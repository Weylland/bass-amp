#include "TunerDisplay.h"
#include "AmpLookAndFeel.h"
#include "Help.h"

namespace
{
    constexpr int padding = 12;
    constexpr float inTuneCents = 3.0f;
    constexpr float defaultReference = 440.0f;

    juce::File referenceFile()
    {
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
            .getChildFile ("Bass Amp").getChildFile ("TunerReference.txt");
    }

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
    muteButton.setColour (juce::TextButton::buttonOnColourId, Palette::ledRed);
    muteButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    muteButton.onClick = [this] { processor.muteOutput.store (muteButton.getToggleState()); };
    muteButton.setTooltip (Help::get ("ui.mute"));
    setTooltip (Help::get ("ui.tuner"));
    addAndMakeVisible (muteButton);

    for (auto* button : { &lowerButton, &raiseButton })
    {
        button->setRepeatSpeed (400, 80);
        button->setTooltip (Help::get ("ui.tuner"));
        addAndMakeVisible (button);
    }

    lowerButton.onClick = [this] { setReference (reference - 1.0f); };
    raiseButton.onClick = [this] { setReference (reference + 1.0f); };

    if (const auto saved = referenceFile().loadFileAsString().getFloatValue(); saved >= 430.0f && saved <= 450.0f)
        reference = saved;

    startTimerHz (20);
}

TunerDisplay::~TunerDisplay()
{
    stopTimer();
}

void TunerDisplay::setReference (float hertz)
{
    reference = juce::jlimit (430.0f, 450.0f, hertz);

    if (referenceFile().getParentDirectory().createDirectory())
        referenceFile().replaceWithText (juce::String (juce::roundToInt (reference)));

    repaint();
}

void TunerDisplay::mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    if (wheel.deltaY != 0.0f)
        setReference (reference + (wheel.deltaY > 0.0f ? 1.0f : -1.0f));
}

void TunerDisplay::mouseDoubleClick (const juce::MouseEvent&)
{
    setReference (defaultReference);
}

void TunerDisplay::timerCallback()
{
    if (const auto frequency = processor.tuner.detectFrequency())
    {
        const auto midi = 69.0f + 12.0f * std::log2 (*frequency / reference);

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
    Draw::unitChassis (g, getLocalBounds().toFloat());

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (20.0f).withKerningFactor (0.12f));
    g.drawText ("TUNER", titleArea, juce::Justification::centredLeft);

    g.setColour (Palette::text);
    g.setFont (Fonts::display (15.0f, false));
    g.drawText ("A = " + juce::String (juce::roundToInt (reference)) + " Hz", referenceArea, juce::Justification::centred);

    Draw::screen (g, screenArea.toFloat(), 8.0f);
    auto area = screenArea.reduced (10, 8);

    const auto nearest = juce::roundToInt (smoothedMidi);
    const auto cents = (smoothedMidi - (float) nearest) * 100.0f;
    const auto inTune = hasPitch && std::abs (cents) <= inTuneCents;
    const auto noteColour = ! hasPitch ? Palette::textDim.withAlpha (0.4f) : (inTune ? Palette::ledGreen : Palette::text);

    auto ledRow = area.removeFromBottom (22);
    auto infoRow = area.removeFromBottom (18);
    const auto name = hasPitch ? noteName (nearest) : juce::String ("--");

    if (inTune)
    {
        g.setColour (Palette::ledGreen.withAlpha (0.18f));
        g.fillEllipse (area.toFloat().withSizeKeepingCentre ((float) area.getHeight() * 1.6f, (float) area.getHeight()));
    }

    g.setColour (noteColour);
    g.setFont (Fonts::display ((float) area.getHeight() * (hasPitch ? 0.85f : 0.4f)));
    g.drawText (name, area, juce::Justification::centred);

    g.setFont (Fonts::display (13.0f, false));
    g.setColour (Palette::textDim);

    if (hasPitch)
    {
        const auto frequency = reference * std::pow (2.0f, (smoothedMidi - 69.0f) / 12.0f);
        const auto centsText = (cents >= 0.0f ? "+" : "") + juce::String (juce::roundToInt (cents)) + " ct";
        g.drawText (juce::String (frequency, 1) + " Hz", infoRow.removeFromLeft (infoRow.getWidth() / 2), juce::Justification::centredLeft);
        g.drawText (centsText, infoRow, juce::Justification::centredRight);
    }

    // 21 LEDs from -50 to +50 cents, the centre one green.
    constexpr int numLeds = 21;
    const auto spacing = (float) ledRow.getWidth() / (float) numLeds;
    const auto litLed = hasPitch ? juce::roundToInt ((juce::jlimit (-50.0f, 50.0f, cents) + 50.0f) / 100.0f * (numLeds - 1)) : -1;

    for (int i = 0; i < numLeds; ++i)
    {
        const auto distance = std::abs (i - numLeds / 2);
        const auto colour = distance == 0 ? Palette::ledGreen : (distance <= 3 ? Palette::ledYellow : Palette::ledRed);
        const juce::Point<float> centre { (float) ledRow.getX() + spacing * ((float) i + 0.5f), (float) ledRow.getCentreY() };
        Draw::led (g, centre, distance == 0 ? 4.0f : 3.0f, colour, i == litLed || (inTune && distance == 0));
    }
}

void TunerDisplay::resized()
{
    auto area = getLocalBounds().reduced (padding + 8, padding).withTrimmedBottom (4);
    auto header = area.removeFromTop (28);
    muteButton.setBounds (header.removeFromRight (64).withSizeKeepingCentre (64, 24));
    titleArea = header;
    area.removeFromTop (6);
    referenceArea = area.removeFromTop (24);
    lowerButton.setBounds (referenceArea.removeFromLeft (32));
    raiseButton.setBounds (referenceArea.removeFromRight (32));
    area.removeFromTop (6);
    screenArea = area;
}
