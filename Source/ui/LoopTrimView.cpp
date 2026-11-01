#include "LoopTrimView.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "../i18n/Language.h"

namespace
{
    constexpr int padding = 16;
    constexpr double minLoopSeconds = 0.1;
    constexpr float grabDistance = 12.0f;
}

LoopTrimView::LoopTrimView (Looper& looperToUse)
    : looper (looperToUse)
{
    resetButton.onClick = [this]
    {
        trim.start = 0;
        trim.end = trim.recorded;
        send();
    };
    doneButton.onClick = [this] { close(); };

    resetButton.setTooltip (Help::get ("ui.looperTrimReset"));
    doneButton.setTooltip (Help::get ("ui.looperTrimDone"));

    addAndMakeVisible (resetButton);
    addAndMakeVisible (doneButton);
}

LoopTrimView::~LoopTrimView()
{
    stopTimer();
}

void LoopTrimView::show (int trackToTrim)
{
    track = trackToTrim;
    trim = looper.getTrim (track);
    dragging = false;
    refreshOverview();
    setVisible (true);
    startTimerHz (30);
}

void LoopTrimView::close()
{
    stopTimer();
    setVisible (false);

    if (onClose != nullptr)
        onClose();
}

void LoopTrimView::timerCallback()
{
    if (! looper.canTrim (track))
    {
        close();
        return;
    }

    if (! dragging)
        trim = looper.getTrim (track);

    // An overdub changes the waveform.
    if (++ticks % 15 == 0)
        refreshOverview();

    repaint();
}

void LoopTrimView::refreshOverview()
{
    overview = looper.getOverview (track, juce::jmax (1, waveArea.getWidth()));
}

float LoopTrimView::xFor (int sample) const
{
    return (float) waveArea.getX() + (float) waveArea.getWidth() * (float) sample / (float) juce::jmax (1, trim.recorded);
}

int LoopTrimView::sampleAt (float x) const
{
    const auto fraction = juce::jlimit (0.0f, 1.0f, (x - (float) waveArea.getX()) / (float) juce::jmax (1, waveArea.getWidth()));
    return juce::roundToInt (fraction * (float) trim.recorded);
}

bool LoopTrimView::nearStart (float x) const
{
    return std::abs (x - xFor (trim.start)) <= std::abs (x - xFor (trim.end));
}

void LoopTrimView::send()
{
    looper.trimTrack (track, trim.start, trim.end);
    repaint();
}

void LoopTrimView::mouseMove (const juce::MouseEvent& event)
{
    const auto x = event.position.x;
    const auto onHandle = waveArea.contains (event.getPosition())
                          && juce::jmin (std::abs (x - xFor (trim.start)), std::abs (x - xFor (trim.end))) <= grabDistance;
    setMouseCursor (onHandle ? juce::MouseCursor::LeftRightResizeCursor : juce::MouseCursor::NormalCursor);
}

void LoopTrimView::mouseDown (const juce::MouseEvent& event)
{
    if (! waveArea.contains (event.getPosition()))
        return;

    dragging = true;
    draggingStart = nearStart (event.position.x);
    mouseDrag (event);
}

void LoopTrimView::mouseDrag (const juce::MouseEvent& event)
{
    if (! dragging)
        return;

    const auto minimum = (int) (looper.getSampleRate() * minLoopSeconds);
    const auto sample = sampleAt (event.position.x);

    if (draggingStart)
        trim.start = juce::jlimit (0, juce::jmax (0, trim.end - minimum), sample);
    else
        trim.end = juce::jlimit (juce::jmin (trim.recorded, trim.start + minimum), trim.recorded, sample);

    send();
}

void LoopTrimView::mouseUp (const juce::MouseEvent&)
{
    dragging = false;
}

void LoopTrimView::resized()
{
    auto area = getLocalBounds().reduced (padding);
    auto header = area.removeFromTop (28);

    doneButton.setBounds (header.removeFromRight (72).withSizeKeepingCentre (72, 26));
    header.removeFromRight (8);
    resetButton.setBounds (header.removeFromRight (72).withSizeKeepingCentre (72, 26));
    titleArea = header;

    area.removeFromTop (4);
    hintArea = area.removeFromTop (20);
    infoArea = area.removeFromBottom (24);
    area.removeFromTop (8);
    area.removeFromBottom (8);
    waveArea = area;

    refreshOverview();
}

void LoopTrimView::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    g.setColour (Palette::panel);
    g.fillRoundedRectangle (bounds, 6.0f);
    g.setColour (Palette::gold.withAlpha (0.4f));
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (18.0f).withKerningFactor (0.1f));
    g.drawText ("TRIM TRACK " + juce::String (track + 1), titleArea, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (Fonts::label (12.0f));
    g.drawText (Language::tr ("Drag the handles to cut the start or the end of the loop. The whole take is kept."),
                hintArea, juce::Justification::centredLeft);

    const auto wave = waveArea.toFloat();
    Draw::screen (g, wave, 4.0f);

    const auto middle = wave.getCentreY();
    const auto halfHeight = wave.getHeight() * 0.45f;
    const auto startX = xFor (trim.start), endX = xFor (trim.end);

    for (size_t i = 0; i < overview.size(); ++i)
    {
        const auto x = wave.getX() + (float) i + 0.5f;
        const auto inside = x >= startX && x <= endX;
        const auto height = juce::jmax (0.5f, juce::jmin (1.0f, overview[i]) * halfHeight);
        g.setColour (inside ? Palette::ledGreen.withAlpha (0.8f) : Palette::textDim.withAlpha (0.35f));
        g.drawVerticalLine ((int) x, middle - height, middle + height);
    }

    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillRect (wave.withRight (startX));
    g.fillRect (wave.withLeft (endX));

    if (const auto progress = looper.getProgress (track); progress > 0.0f)
    {
        const auto playX = xFor (trim.start + (int) (progress * (float) (trim.end - trim.start)));
        g.setColour (Palette::text.withAlpha (0.7f));
        g.drawVerticalLine ((int) playX, wave.getY(), wave.getBottom());
    }

    for (const auto x : { startX, endX })
    {
        g.setColour (Palette::gold);
        g.fillRect (x - 1.0f, wave.getY(), 2.0f, wave.getHeight());
        g.fillRoundedRectangle (juce::Rectangle<float> (12.0f, 22.0f).withCentre ({ x, wave.getY() + 14.0f }), 3.0f);
    }

    const auto rate = looper.getSampleRate();
    auto seconds = [rate] (int samples) { return juce::String ((double) samples / rate, 2) + " s"; };
    g.setColour (Palette::text);
    g.setFont (Fonts::label (13.0f));
    g.drawText (seconds (trim.start) + "  >  " + seconds (trim.end) + "     LOOP " + seconds (trim.end - trim.start),
                infoArea, juce::Justification::centredLeft);
}
