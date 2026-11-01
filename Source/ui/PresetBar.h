#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "PresetsButton.h"
#include "../presets/PresetManager.h"

// Preset selector: factory and user presets, previous/next, saving the current sound, and a
// menu to rename, delete, import and export presets. A gold star marks a sound changed since its
// preset was loaded; loading another one then asks first.
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
    void save (const juce::String& name);
    void showMenu();
    void askNameAndRename (const juce::File&);
    void confirmAndDelete (const juce::File&);
    void chooseAndImport();
    void chooseAndExport();

    PresetManager& presets;
    juce::ComboBox presetBox;
    juce::TextButton previousButton { "<" }, nextButton { ">" }, saveButton { "SAVE" };
    PresetsButton menuButton;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Array<juce::File> userPresets;
    juce::String shownName;
    int loadedItemId = 0;
    bool modified = false;
};
