#include "LevelMeters.h"
#include "AmpLookAndFeel.h"

namespace
{
    constexpr int padding = 12;
    constexpr int clipLampHeight = 22;
    constexpr int numSegments = 24;
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
    Draw::unitChassis (g, getLocalBounds().toFloat());

    auto area = getLocalBounds().reduced (padding + 8, padding).withTrimmedBottom (4);
    g.setColour (Palette::gold);
    g.setFont (Fonts::display (20.0f).withKerningFactor (0.12f));
    g.drawText ("LEVELS", area.removeFromTop (28), juce::Justification::centredLeft);
    area.removeFromTop (8);

    Draw::screen (g, area.toFloat(), 8.0f);
    area = area.reduced (12, 8);

    const auto now = juce::Time::getMillisecondCounterHiRes() * 0.001;
    auto labels = area.removeFromBottom (16);
    const auto columnWidth = area.getWidth() / 4;

    auto inColumn = area.removeFromLeft (columnWidth);
    auto scaleColumn = area.removeFromLeft (columnWidth);
    auto outColumn = area.removeFromLeft (columnWidth);
    auto compColumn = area;

    drawMeter (g, inColumn, input, now);
    drawMeter (g, outColumn, output, now);
    drawGainReduction (g, compColumn);

    const auto bar = scaleColumn.withTrimmedTop (clipLampHeight + 4).toFloat();
    g.setFont (Fonts::display (10.0f, false));
    g.setColour (Palette::textDim);

    for (const auto db : { 0.0f, -6.0f, -12.0f, -24.0f, -36.0f, -48.0f })
    {
        const auto y = bar.getBottom() - bar.getHeight() * proportionOf (db);
        g.drawText (juce::String (juce::roundToInt (db)), juce::Rectangle<float> (bar.getX(), y - 6.0f, bar.getWidth(), 12.0f),
                    juce::Justification::centred);
    }

    g.setFont (Fonts::label (10.5f));
    g.setColour (Palette::text.withAlpha (0.8f));
    g.drawText ("IN", labels.removeFromLeft (columnWidth), juce::Justification::centred);
    labels.removeFromLeft (columnWidth);
    g.drawText ("OUT", labels.removeFromLeft (columnWidth), juce::Justification::centred);
    g.drawText ("COMP", labels, juce::Justification::centred);
}

void LevelMeters::drawMeter (juce::Graphics& g, juce::Rectangle<int> area, const Meter& meter, double now) const
{
    const auto isClipping = now < meter.clipUntil;
    const auto lamp = area.removeFromTop (clipLampHeight).toFloat().withSizeKeepingCentre ((float) area.getWidth() - 6.0f, 16.0f);

    if (isClipping)
    {
        g.setColour (clipColour.withAlpha (0.35f));
        g.fillRoundedRectangle (lamp.expanded (3.0f), 5.0f);
    }

    g.setColour (isClipping ? clipColour : juce::Colour (0xff2a1414));
    g.fillRoundedRectangle (lamp, 3.0f);
    g.setColour (isClipping ? juce::Colours::white : clipColour.withAlpha (0.45f));
    g.setFont (Fonts::label (10.0f));
    g.drawText ("CLIP", lamp, juce::Justification::centred);

    drawSegments (g, area.withTrimmedTop (4), proportionOf (meter.levelDb), true);
}

void LevelMeters::drawGainReduction (juce::Graphics& g, juce::Rectangle<int> area) const
{
    const auto readout = area.removeFromTop (clipLampHeight);
    g.setColour (Palette::gold);
    g.setFont (Fonts::display (11.0f));
    g.drawText (gainReductionDb < 0.05f ? juce::String ("0.0") : "-" + juce::String (gainReductionDb, 1), readout, juce::Justification::centred);

    drawSegments (g, area.withTrimmedTop (4), juce::jlimit (0.0f, 1.0f, gainReductionDb / maxGainReductionDb), false);
}

// A column of LED segments. Level meters fill upwards in green, yellow and red;
// gain reduction fills downwards in gold.
void LevelMeters::drawSegments (juce::Graphics& g, juce::Rectangle<int> area, float amount, bool isLevel) const
{
    const auto bar = area.toFloat().withSizeKeepingCentre (14.0f, (float) area.getHeight());
    const auto segmentHeight = bar.getHeight() / (float) numSegments;

    for (int i = 0; i < numSegments; ++i)
    {
        const auto proportion = ((float) i + 0.5f) / (float) numSegments;
        const auto db = floorDb * (1.0f - proportion);
        const auto colour = ! isLevel ? Palette::gold : (db > -3.0f ? clipColour : (db > -12.0f ? hotColour : okColour));
        const auto lit = proportion <= amount;
        const auto y = isLevel ? bar.getBottom() - segmentHeight * (float) (i + 1) : bar.getY() + segmentHeight * (float) i;
        const auto segment = juce::Rectangle<float> (bar.getX(), y, bar.getWidth(), segmentHeight - 1.5f);

        if (lit)
        {
            g.setColour (colour.withAlpha (0.25f));
            g.fillRoundedRectangle (segment.expanded (2.5f, 0.5f), 2.0f);
        }

        g.setColour (lit ? colour : colour.withMultipliedBrightness (0.18f));
        g.fillRoundedRectangle (segment, 1.0f);
    }
}
