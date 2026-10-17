#include "LevelMeters.h"
#include "AmpLookAndFeel.h"

namespace
{
    constexpr int padding = 12;
    constexpr float floorDb = -60.0f;
    constexpr double clipHoldSeconds = 2.0;
    constexpr float decayDbPerFrame = 1.5f;
    constexpr float maxGainReductionDb = 24.0f;

    // The interface's converter clips at 0 dBFS: warn slightly before.
    constexpr float inputClipLevel = 0.98f;
    constexpr float outputClipLevel = 1.0f;

    const juce::Colour clipColour { 0xffff4b4b };
    const juce::Colour okColour { 0xff5fd68a };
    const juce::Colour hotColour { 0xffe0c04f };

    float proportionOf (float db)
    {
        return juce::jlimit (0.0f, 1.0f, (db - floorDb) / -floorDb);
    }
}

LevelMeters::LevelMeters (BassAmpProcessor& processorToUse)
    : processor (processorToUse)
{
    startTimerHz (30);
}

LevelMeters::~LevelMeters()
{
    stopTimer();
}

void LevelMeters::update (Meter& meter, float peak, float clipThreshold, double now)
{
    const auto peakDb = juce::Decibels::gainToDecibels (peak, -100.0f);
    meter.levelDb = juce::jmax (peakDb, meter.levelDb - decayDbPerFrame);

    if (peak >= clipThreshold)
        meter.clipUntil = now + clipHoldSeconds;
}

void LevelMeters::timerCallback()
{
    const auto now = juce::Time::getMillisecondCounterHiRes() * 0.001;
    update (input, processor.inputMeter.takePeak(), inputClipLevel, now);
    update (output, processor.outputMeter.takePeak(), outputClipLevel, now);

    const auto reduction = processor.getCompressorGainReductionDb();
    gainReductionDb = reduction > gainReductionDb ? reduction : gainReductionDb + 0.25f * (reduction - gainReductionDb);

    repaint();
}

void LevelMeters::mouseDown (const juce::MouseEvent&)
{
    input.clipUntil = output.clipUntil = 0.0;
    repaint();
}

void LevelMeters::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (Palette::panel);
    g.fillRoundedRectangle (bounds, 10.0f);
    g.setColour (Palette::textDim.withAlpha (0.35f));
    g.drawRoundedRectangle (bounds, 10.0f, 1.0f);

    auto area = getLocalBounds().reduced (padding);
    g.setColour (Palette::text);
    g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    g.drawText ("LEVELS", area.removeFromTop (26), juce::Justification::centredLeft);
    area.removeFromTop (4);

    const auto now = juce::Time::getMillisecondCounterHiRes() * 0.001;
    const auto rowHeight = area.getHeight() / 3;
    drawMeter (g, area.removeFromTop (rowHeight), "IN", input, now);
    drawMeter (g, area.removeFromTop (rowHeight), "OUT", output, now);
    drawGainReduction (g, area);
}

void LevelMeters::drawMeter (juce::Graphics& g, juce::Rectangle<int> area, const juce::String& name,
                             const Meter& meter, double now) const
{
    g.setColour (Palette::textDim);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText (name, area.removeFromLeft (34), juce::Justification::centredLeft);

    const auto isClipping = now < meter.clipUntil;
    auto led = area.removeFromRight (46).toFloat().withSizeKeepingCentre (42.0f, 18.0f);
    g.setColour (isClipping ? clipColour : Palette::track);
    g.fillRoundedRectangle (led, 4.0f);
    g.setColour (isClipping ? juce::Colours::white : Palette::textDim);
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText ("CLIP", led, juce::Justification::centred);

    const auto bar = area.toFloat().withSizeKeepingCentre ((float) area.getWidth() - 8.0f, 10.0f);
    g.setColour (Palette::track);
    g.fillRoundedRectangle (bar, 3.0f);

    const auto level = proportionOf (meter.levelDb);
    const auto colour = meter.levelDb > -1.0f ? clipColour : (meter.levelDb > -6.0f ? hotColour : okColour);
    g.setColour (colour);
    g.fillRoundedRectangle (bar.withWidth (bar.getWidth() * level), 3.0f);

    g.setColour (Palette::textDim.withAlpha (0.6f));
    const auto sixDbMark = bar.getX() + bar.getWidth() * proportionOf (-6.0f);
    g.fillRect (sixDbMark, bar.getY() - 2.0f, 1.0f, bar.getHeight() + 4.0f);
}

void LevelMeters::drawGainReduction (juce::Graphics& g, juce::Rectangle<int> area) const
{
    g.setColour (Palette::textDim);
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("COMP", area.removeFromLeft (34), juce::Justification::centredLeft);

    g.setFont (juce::FontOptions (11.0f));
    const auto reductionText = gainReductionDb < 0.05f ? juce::String ("0.0 dB") : "-" + juce::String (gainReductionDb, 1) + " dB";
    g.drawText (reductionText, area.removeFromRight (46), juce::Justification::centred);

    const auto bar = area.toFloat().withSizeKeepingCentre ((float) area.getWidth() - 8.0f, 10.0f);
    g.setColour (Palette::track);
    g.fillRoundedRectangle (bar, 3.0f);

    const auto amount = juce::jlimit (0.0f, 1.0f, gainReductionDb / maxGainReductionDb);
    g.setColour (Palette::amp);
    g.fillRoundedRectangle (bar.withTrimmedLeft (bar.getWidth() * (1.0f - amount)), 3.0f);
}
