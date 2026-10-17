#include "PluginEditor.h"
#include "ParameterIds.h"

namespace
{
    constexpr int defaultWidth = 1240;
    constexpr int defaultHeight = 760;
    constexpr int headerHeight = 48;
    constexpr int margin = 14;
    constexpr int monitorWidth = 300;
}

BassAmpEditor::BassAmpEditor (BassAmpProcessor& audioProcessor)
    : AudioProcessorEditor (audioProcessor),
      bassAmp (audioProcessor),
      tuner (audioProcessor),
      meters (audioProcessor),
      gateSection (audioProcessor.parameters, "GATE", Palette::gate,
                   { { ParamIds::gateThreshold, "Threshold" }, { ParamIds::gateRelease, "Release" } },
                   ParamIds::gateOn),
      compressorSection (audioProcessor.parameters, "COMPRESSOR", Palette::compressor,
                         { { ParamIds::compThreshold, "Threshold" }, { ParamIds::compRatio, "Ratio" },
                           { ParamIds::compAttack, "Attack" }, { ParamIds::compRelease, "Release" },
                           { ParamIds::compMakeup, "Makeup" } },
                         ParamIds::compOn),
      octaverSection (audioProcessor.parameters, "OCTAVER", Palette::octaver,
                      { { ParamIds::octSub, "Sub" }, { ParamIds::octDry, "Dry" } },
                      ParamIds::octOn),
      envFilterSection (audioProcessor.parameters, "ENV FILTER", Palette::envFilter,
                        { { ParamIds::envSensitivity, "Sensitivity" }, { ParamIds::envResonance, "Resonance" },
                          { ParamIds::envMix, "Mix" } },
                        ParamIds::envOn),
      fuzzSection (audioProcessor.parameters, "FUZZ", Palette::fuzz,
                   { { ParamIds::fuzzAmount, "Fuzz" }, { ParamIds::fuzzTone, "Tone" }, { ParamIds::fuzzBlend, "Blend" } },
                   ParamIds::fuzzOn),
      distortionSection (audioProcessor.parameters, "DISTO", Palette::distortion,
                         { { ParamIds::distDrive, "Drive" }, { ParamIds::distTone, "Tone" }, { ParamIds::distBlend, "Blend" } },
                         ParamIds::distOn),
      ampSection (audioProcessor.parameters, "AMP", Palette::amp,
                  { { ParamIds::ampGain, "Gain" }, { ParamIds::ampBass, "Bass" }, { ParamIds::ampMid, "Mid" },
                    { ParamIds::ampTreble, "Treble" }, { ParamIds::ampMaster, "Master" } }),
      chorusSection (audioProcessor.parameters, "CHORUS", Palette::chorus,
                     { { ParamIds::chorusRate, "Rate" }, { ParamIds::chorusDepth, "Depth" }, { ParamIds::chorusMix, "Mix" } },
                     ParamIds::chorusOn),
      delaySection (audioProcessor.parameters, "DELAY", Palette::delay,
                    { { ParamIds::delayTime, "Time" }, { ParamIds::delayFeedback, "Feedback" }, { ParamIds::delayMix, "Mix" } },
                    ParamIds::delayOn),
      reverbSection (audioProcessor.parameters, "REVERB", Palette::reverb,
                     { { ParamIds::reverbSize, "Size" }, { ParamIds::reverbDamping, "Damping" }, { ParamIds::reverbMix, "Mix" } },
                     ParamIds::reverbOn)
{
    addAndMakeVisible (tuner);
    addAndMakeVisible (meters);

    for (auto* section : { &gateSection, &compressorSection, &octaverSection, &envFilterSection, &fuzzSection,
                           &distortionSection, &ampSection, &chorusSection, &delaySection, &reverbSection })
        addAndMakeVisible (section);

    setLookAndFeel (&lookAndFeel);
    shownDeviceInfo = bassAmp.audioDeviceInfo;

    setResizable (true, true);
    setResizeLimits (1000, 613, 1920, 1177);
    getConstrainer()->setFixedAspectRatio ((double) defaultWidth / defaultHeight);
    setSize (defaultWidth, defaultHeight);

    startTimerHz (2);
}

BassAmpEditor::~BassAmpEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void BassAmpEditor::timerCallback()
{
    if (bassAmp.audioDeviceInfo != shownDeviceInfo)
    {
        shownDeviceInfo = bassAmp.audioDeviceInfo;
        repaint (getLocalBounds().removeFromTop (headerHeight));
    }
}

void BassAmpEditor::paint (juce::Graphics& g)
{
    g.fillAll (Palette::background);

    auto header = getLocalBounds().removeFromTop (headerHeight).reduced (margin, 0);

    g.setColour (Palette::text);
    g.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    g.drawText ("BASS AMP", header, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (juce::FontOptions (12.0f));
    g.drawText (shownDeviceInfo, header, juce::Justification::centredRight);
}

void BassAmpEditor::layoutRow (juce::Rectangle<int> area, std::initializer_list<Section*> sections)
{
    auto totalWeight = 0;

    for (auto* section : sections)
        totalWeight += section->getNumKnobs() + 1;

    const auto available = area.getWidth() - margin * ((int) sections.size() - 1);
    auto remainingWeight = totalWeight;
    auto remainingWidth = available;

    for (auto* section : sections)
    {
        const auto weight = section->getNumKnobs() + 1;
        const auto width = remainingWidth * weight / remainingWeight;
        section->setBounds (area.removeFromLeft (width));
        area.removeFromLeft (margin);
        remainingWidth -= width;
        remainingWeight -= weight;
    }
}

void BassAmpEditor::resized()
{
    auto area = getLocalBounds().reduced (margin);
    area.removeFromTop (headerHeight - margin);

    const auto rowHeight = (area.getHeight() - 2 * margin) / 3;
    const auto scale = (float) getWidth() / (float) defaultWidth;

    auto top = area.removeFromTop (rowHeight);
    tuner.setBounds (top.removeFromLeft (juce::roundToInt (monitorWidth * scale)));
    top.removeFromLeft (margin);
    meters.setBounds (top.removeFromLeft (juce::roundToInt (monitorWidth * scale)));
    top.removeFromLeft (margin);
    layoutRow (top, { &gateSection, &compressorSection });
    area.removeFromTop (margin);

    layoutRow (area.removeFromTop (rowHeight), { &octaverSection, &envFilterSection, &fuzzSection, &distortionSection });
    area.removeFromTop (margin);

    layoutRow (area, { &ampSection, &chorusSection, &delaySection, &reverbSection });
}
