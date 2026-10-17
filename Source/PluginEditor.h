#pragma once

#include "PluginProcessor.h"
#include "ui/AmpLookAndFeel.h"
#include "ui/Section.h"

class BassAmpEditor : public juce::AudioProcessorEditor
{
public:
    explicit BassAmpEditor (BassAmpProcessor&);
    ~BassAmpEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AmpLookAndFeel lookAndFeel;

    Section ampSection;
    Section distortionSection;
    Section chorusSection;
    Section reverbSection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassAmpEditor)
};
