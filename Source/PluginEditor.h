#pragma once

#include "PluginProcessor.h"
#include "ui/AmpLookAndFeel.h"
#include "ui/LevelMeters.h"
#include "ui/PresetBar.h"
#include "ui/Section.h"
#include "ui/TunerDisplay.h"

class BassAmpEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit BassAmpEditor (BassAmpProcessor&);
    ~BassAmpEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    static void layoutRow (juce::Rectangle<int> area, std::initializer_list<Section*> sections);

    BassAmpProcessor& bassAmp;
    AmpLookAndFeel lookAndFeel;
    juce::String shownDeviceInfo;

    PresetBar presetBar;
    TunerDisplay tuner;
    LevelMeters meters;

    Section gateSection;
    Section compressorSection;
    Section octaverSection;
    Section envFilterSection;
    Section fuzzSection;
    Section distortionSection;
    Section ampSection;
    Section chorusSection;
    Section delaySection;
    Section reverbSection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassAmpEditor)
};
