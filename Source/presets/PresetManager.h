#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Factory presets come from code; user presets are XML files of the whole parameter state.
// A preset is shared as a zip holding it with its IR and capture files. Message thread only.
class PresetManager
{
public:
    explicit PresetManager (juce::AudioProcessorValueTreeState&, juce::File userDirectory = getDefaultUserDirectory());

    static juce::File getDefaultUserDirectory();
    static constexpr const char* fileExtension = ".bassamp";
    static inline const juce::Identifier impulseResponseProperty { "cabinetImpulseResponse" };
    static inline const juce::Identifier captureProperty { "namCapture" };

    int getNumFactoryPresets() const;
    juce::String getFactoryPresetName (int index) const;
    juce::Array<juce::File> getUserPresets() const;
    juce::File getUserPresetFile (const juce::String& name) const;
    juce::File getUserDirectory() const { return userDirectory; }

    // The user preset loaded now, if any.
    juce::File getCurrentUserPreset() const;
    juce::File renameUserPreset (const juce::File&, const juce::String& newName);
    bool deleteUserPreset (const juce::File&);

    // Zip of the current sound and its IR and capture files.
    bool exportPreset (const juce::File& zipFile) const;

    // A .bassamp file, or a zip from exportPreset, added to the user presets: returns the new
    // preset file, or an invalid file if it isn't a preset.
    juce::File importPreset (const juce::File&);

    void loadFactoryPreset (int index);
    bool loadUserPreset (const juce::File&);
    juce::File saveUserPreset (const juce::String& name);

    // Replaces the whole state. Parameters the tree lacks, from a preset or session saved by an
    // older version, go to their default instead of keeping their current value.
    void restoreState (juce::ValueTree);

    juce::String getCurrentPresetName() const;
    int getCurrentFactoryIndex() const;

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
    std::atomic<bool> referencePending { true };
    std::vector<float> reference;
    juce::String referenceChain;
};
