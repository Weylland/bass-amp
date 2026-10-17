#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Factory presets come from code; user presets are XML files of the whole parameter state.
// Every method runs on the message thread.
class PresetManager
{
public:
    explicit PresetManager (juce::AudioProcessorValueTreeState&, juce::File userDirectory = getDefaultUserDirectory());

    static juce::File getDefaultUserDirectory();
    static constexpr const char* fileExtension = ".bassamp";

    int getNumFactoryPresets() const;
    juce::String getFactoryPresetName (int index) const;
    juce::Array<juce::File> getUserPresets() const;

    void loadFactoryPreset (int index);
    bool loadUserPreset (const juce::File&);
    juce::File saveUserPreset (const juce::String& name);

    juce::String getCurrentPresetName() const;
    int getCurrentFactoryIndex() const { return currentFactoryIndex; }

private:
    void setCurrentPresetName (const juce::String&);

    juce::AudioProcessorValueTreeState& state;
    juce::File userDirectory;
    int currentFactoryIndex = -1;
};
