#include "MetronomeUnit.h"
#include "AmpLookAndFeel.h"
#include "Help.h"

namespace
{
    constexpr int sidePadding = 14;
    constexpr int titleHeight = 22;
    constexpr int gap = 8;
}

MetronomeUnit::MetronomeUnit (BassAmpProcessor& processor)
    : metronome (processor.metronome)
{
    onSwitch.onClick = [this] { metronome.enabled.store (onSwitch.getToggleState()); };

    tempo.setRange (40.0, 240.0, 1.0);
    tempo.setValue (metronome.bpm.load(), juce::dontSendNotification);
    tempo.setTextValueSuffix (" BPM");
    tempo.setPopupDisplayEnabled (true, true, nullptr);
    tempo.onValueChange = [this]
    {
        metronome.bpm.store ((float) tempo.getValue());
        repaint (screenArea);
    };

    tapButton.onClick = [this]
    {
        if (const auto beat = tapTempo.tap())
            tempo.setValue (60000.0 / *beat);
    };

    volume.setRange (0.0, 1.0);
    volume.setValue (metronome.volume.load(), juce::dontSendNotification);
    volume.onValueChange = [this] { metronome.volume.store ((float) volume.getValue()); };

    setTooltip (Help::get ("ui.metronome"));
    onSwitch.setTooltip (Help::get ("ui.metronomeOn"));
    tempo.setTooltip (Help::get ("ui.metronomeTempo"));
    tapButton.setTooltip (Help::get ("ui.metronomeTap"));
    volume.setTooltip (Help::get ("ui.metronomeVolume"));

    for (auto* knob : { &tempo, &volume })
        knob->getProperties().set (KnobStyle::property, KnobStyle::pedal);

    for (auto* component : std::initializer_list<juce::Component*> { &onSwitch, &tapButton, &tempo, &volume })
        addAndMakeVisible (component);

    startTimerHz (30);
}

MetronomeUnit::~MetronomeUnit()
{
    stopTimer();
}

void MetronomeUnit::timerCallback()
{
    const auto followsHost = metronome.followsHost.load();

    if (followsHost != ! tempo.isEnabled())
    {
        for (auto* control : std::initializer_list<juce::Component*> { &tempo, &tapButton })
        {
            control->setEnabled (! followsHost);
            control->setAlpha (followsHost ? 0.35f : 1.0f);
        }
    }

    if (followsHost)
        tempo.setValue (metronome.bpm.load(), juce::dontSendNotification);

    if (const auto beat = metronome.getBeat(); beat != shownBeat)
    {
        shownBeat = beat;
        repaint (screenArea);
    }
}

void MetronomeUnit::resized()
{
    auto area = getLocalBounds().reduced (sidePadding, 8).withTrimmedBottom (4);
    titleArea = area.removeFromTop (titleHeight);
    auto row = area.withSizeKeepingCentre (area.getWidth(), 30);

    onSwitch.setBounds (row.removeFromLeft (56).reduced (0, 3));
    row.removeFromLeft (gap);

    volume.setBounds (row.removeFromRight (40).withSizeKeepingCentre (40, 40));
    volumeLabelArea = titleArea.withLeft (volume.getX() - 10).withRight (volume.getRight() + 10);
    row.removeFromRight (gap);
    tapButton.setBounds (row.removeFromRight (46).reduced (0, 2));
    row.removeFromRight (gap);
    tempo.setBounds (row.removeFromRight (40).withSizeKeepingCentre (40, 40));
    tempoLabelArea = titleArea.withLeft (tempo.getX() - 10).withRight (tempo.getRight() + 10);
    row.removeFromRight (gap);

    screenArea = row;
}

void MetronomeUnit::paint (juce::Graphics& g)
{
    Draw::unitChassis (g, getLocalBounds().toFloat());

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (15.0f).withKerningFactor (0.12f));
    g.drawText ("METRONOME", titleArea, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (Fonts::label (10.0f));
    g.drawText ("TEMPO", tempoLabelArea, juce::Justification::centredBottom);
    g.drawText ("VOL", volumeLabelArea, juce::Justification::centredBottom);

    Draw::screen (g, screenArea.toFloat(), 5.0f);
    auto inner = screenArea.reduced (10, 2);

    g.setColour (Palette::text);
    g.setFont (Fonts::display (17.0f));
    g.drawText (juce::String (juce::roundToInt (metronome.bpm.load())), inner.removeFromLeft (36), juce::Justification::centredLeft);
    g.setColour (Palette::textDim);
    g.setFont (Fonts::label (10.0f));
    g.drawText ("BPM", inner.removeFromLeft (28), juce::Justification::centredLeft);

    const auto beats = juce::jmax (1, metronome.beatsPerBar.load());
    const auto spacing = juce::jmin (16.0f, (float) inner.getWidth() / (float) beats);

    for (int beat = 0; beat < beats; ++beat)
    {
        const auto centre = juce::Point<float> ((float) inner.getX() + spacing * ((float) beat + 0.5f), (float) inner.getCentreY());
        Draw::led (g, centre, beat == 0 ? 4.5f : 3.5f, beat == 0 ? Palette::ledRed : Palette::ledGreen, beat == shownBeat);
    }
}
