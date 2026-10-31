#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Factory presets come from code; user presets are XML files of the whole parameter state.
// Every method runs on the message thread, except loading a factory preset, which a MIDI
// program change also does from the audio thread.
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

    // Whether the sound differs from the preset last loaded or saved. The reference is taken
    // at the first call after a load, so a load from any thread is safe.
    bool isModified();

    // Takes the current sound as the reference, as after a session is restored.
    void markUnmodified() { referencePending = true; }

private:
    void setCurrentPresetName (const juce::String&);
    std::vector<float> currentValues() const;

    juce::AudioProcessorValueTreeState& state;
    juce::File userDirectory;
    int currentFactoryIndex = -1;
    std::atomic<bool> referencePending { true };
    std::vector<float> reference;
    juce::String referenceChain;
};
