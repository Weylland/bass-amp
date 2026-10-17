#include "PluginEditor.h"
#include "ParameterIds.h"

namespace
{
    constexpr int headerHeight = 56;
    constexpr int margin = 16;
}

BassAmpEditor::BassAmpEditor (BassAmpProcessor& audioProcessor)
    : AudioProcessorEditor (audioProcessor),
      ampSection (audioProcessor.parameters, "AMP", Palette::amp,
                  { { ParamIds::ampGain, "Gain" }, { ParamIds::ampBass, "Bass" }, { ParamIds::ampMid, "Mid" },
                    { ParamIds::ampTreble, "Treble" }, { ParamIds::ampMaster, "Master" } }),
      distortionSection (audioProcessor.parameters, "DISTO", Palette::distortion,
                         { { ParamIds::distDrive, "Drive" }, { ParamIds::distTone, "Tone" }, { ParamIds::distBlend, "Blend" } },
                         ParamIds::distOn),
      chorusSection (audioProcessor.parameters, "CHORUS", Palette::chorus,
                     { { ParamIds::chorusRate, "Rate" }, { ParamIds::chorusDepth, "Depth" }, { ParamIds::chorusMix, "Mix" } },
                     ParamIds::chorusOn),
      reverbSection (audioProcessor.parameters, "REVERB", Palette::reverb,
                     { { ParamIds::reverbSize, "Size" }, { ParamIds::reverbDamping, "Damping" }, { ParamIds::reverbMix, "Mix" } },
                     ParamIds::reverbOn)
{
    setLookAndFeel (&lookAndFeel);

    for (auto* section : { &ampSection, &distortionSection, &chorusSection, &reverbSection })
        addAndMakeVisible (section);

    setSize (900, 520);
}

BassAmpEditor::~BassAmpEditor()
{
    setLookAndFeel (nullptr);
}

void BassAmpEditor::paint (juce::Graphics& g)
{
    g.fillAll (Palette::background);

    auto header = getLocalBounds().removeFromTop (headerHeight).reduced (margin, 0);

    g.setColour (Palette::text);
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawText ("BASS AMP", header, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (juce::FontOptions (12.0f));
    g.drawText ("DISTO  >  AMP  >  CHORUS  >  REVERB", header, juce::Justification::centredRight);
}

void BassAmpEditor::resized()
{
    auto area = getLocalBounds().reduced (margin);
    area.removeFromTop (headerHeight - margin);

    ampSection.setBounds (area.removeFromTop (area.getHeight() / 2).withTrimmedBottom (margin / 2));
    area.removeFromTop (margin / 2);

    const auto effectWidth = (area.getWidth() - 2 * margin) / 3;
    distortionSection.setBounds (area.removeFromLeft (effectWidth));
    area.removeFromLeft (margin);
    chorusSection.setBounds (area.removeFromLeft (effectWidth));
    area.removeFromLeft (margin);
    reverbSection.setBounds (area);
}
