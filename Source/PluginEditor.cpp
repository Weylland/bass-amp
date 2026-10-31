#include "PluginEditor.h"

namespace
{
    constexpr double minScale = 0.6;
    constexpr double maxScale = 1.6;

    // The largest scale, up to 1, at which the window fits on the main screen.
    double initialScale()
    {
        const auto* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();

        if (display == nullptr)
            return 1.0;

        const auto area = display->userArea;
        return juce::jlimit (minScale, 1.0, juce::jmin ((area.getWidth() - 40.0) / MainPanel::designWidth,
                                                        (area.getHeight() - 90.0) / MainPanel::designHeight));
    }
}

BassAmpEditor::BassAmpEditor (BassAmpProcessor& audioProcessor)
    : AudioProcessorEditor (audioProcessor),
      panel (audioProcessor)
{
    addAndMakeVisible (panel);
    setLookAndFeel (&lookAndFeel);

    setResizable (true, true);
    setResizeLimits (juce::roundToInt (MainPanel::designWidth * minScale), juce::roundToInt (MainPanel::designHeight * minScale),
                     juce::roundToInt (MainPanel::designWidth * maxScale), juce::roundToInt (MainPanel::designHeight * maxScale));
    getConstrainer()->setFixedAspectRatio ((double) MainPanel::designWidth / MainPanel::designHeight);

    const auto scale = initialScale();
    setSize (juce::roundToInt (MainPanel::designWidth * scale), juce::roundToInt (MainPanel::designHeight * scale));
}

BassAmpEditor::~BassAmpEditor()
{
    setLookAndFeel (nullptr);
}

void BassAmpEditor::paint (juce::Graphics& g)
{
    g.fillAll (Palette::background);
}

void BassAmpEditor::resized()
{
    const auto scale = (float) getWidth() / (float) MainPanel::designWidth;
    panel.setTransform (juce::AffineTransform::scale (scale));
}
