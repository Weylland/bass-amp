#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../presets/PresetManager.h"

// Preset selector: factory and user presets, previous/next, and saving the current sound.
class PresetBar : public juce::Component, private juce::Timer
{
public:
    explicit PresetBar (PresetManager&);
    ~PresetBar() override;

    void resized() override;

private:
    void timerCallback() override;
    void refreshList();
    void loadItem (int itemId);
    void step (int delta);
    void askNameAndSave();

    PresetManager& presets;
    juce::ComboBox presetBox;
    juce::TextButton previousButton { "<" }, nextButton { ">" }, saveButton { "Save" };
    juce::Array<juce::File> userPresets;
    juce::String shownName;
};
