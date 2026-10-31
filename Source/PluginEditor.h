#pragma once

#include "PluginProcessor.h"
#include "ui/AmpLookAndFeel.h"
#include "ui/MainPanel.h"

// Hosts the rig drawn at its design size and scales it to the window, keeping the aspect ratio.
class BassAmpEditor : public juce::AudioProcessorEditor
{
public:
    explicit BassAmpEditor (BassAmpProcessor&);
    ~BassAmpEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AmpLookAndFeel lookAndFeel;
    MainPanel panel;
};
