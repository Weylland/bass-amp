#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../presets/PresetManager.h"

// Preset selector: factory and user presets, previous/next, and saving the current sound. A gold
// star marks a sound changed since its preset was loaded; loading another one then asks first.
class PresetBar : public juce::Component, private juce::Timer
{
public:
    explicit PresetBar (PresetManager&);
    ~PresetBar() override;

    void resized() override;
    void paintOverChildren (juce::Graphics&) override;

private:
    void timerCallback() override;
    void refreshList();
    void loadItem (int itemId);
    void confirmAndLoad (int itemId);
    void step (int delta);
    void askNameAndSave();

    PresetManager& presets;
    juce::ComboBox presetBox;
    juce::TextButton previousButton { "<" }, nextButton { ">" }, saveButton { "SAVE" };
    juce::Array<juce::File> userPresets;
    juce::String shownName;
    int loadedItemId = 0;
    bool modified = false;
};
