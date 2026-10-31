#pragma once

#include "PluginProcessor.h"
#include "ui/AmpLookAndFeel.h"
#include "ui/MainPanel.h"

// Hosts the rig drawn at its design size and scales it to the window, keeping the aspect ratio.
// In the standalone app the window can go full screen, from the header button or F11; Escape
// leaves it. The rig is then centred on the screen. Changing the language rebuilds the rig,
// so every text is read again.
class BassAmpEditor : public juce::AudioProcessorEditor, private juce::KeyListener
{
public:
    explicit BassAmpEditor (BassAmpProcessor&);
    ~BassAmpEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void parentHierarchyChanged() override;
    using juce::AudioProcessorEditor::keyPressed;

private:
    bool keyPressed (const juce::KeyPress&, juce::Component*) override;
    bool isFullScreen() const;
    void setFullScreen (bool);
    void setWindowLimits (bool fullScreen);
    void createPanel();

    bool standalone;
    juce::Component::SafePointer<juce::Component> window;
    AmpLookAndFeel lookAndFeel;
    std::unique_ptr<MainPanel> panel;
};
