#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// MIDI control. A CC drives any parameter (switches flip at 64); a note toggles a switch or
// steps through a choice; a program change loads a factory preset. Actions, such as the
// looper's buttons, fire on a note or when a CC goes over 64. Mappings are learnt from the
// next CC or note after learn(), and saved in a file shared by every session rather than in
// presets. Without a saved map, CC 11 (expression pedal) drives the wah.
class MidiLearn : private juce::AsyncUpdater
{
public:
    using Action = std::pair<const char*, const char*>;   // { id, name }

    MidiLearn (juce::AudioProcessorValueTreeState&, juce::File mapFile, const std::vector<Action>& actions = {});
    ~MidiLearn() override;

    static juce::File getDefaultFile();

    // Called on the message thread for a received program change.
    std::function<void (int)> onProgramChange;

    // Called on the audio thread with the index of a fired action.
    std::function<void (int)> onAction;

    // Audio thread.
    void process (const juce::MidiBuffer&);

    // Message thread. Targets are parameter or action IDs.
    bool isLearnable (const juce::String& id) const { return indexOf (id) >= 0; }
    juce::String getName (const juce::String& id) const;
    void learn (const juce::String& paramId);
    void cancelLearning();
    juce::String getLearningParameter() const;
    void forget (const juce::String& paramId);
    juce::String describe (const juce::String& paramId) const;
    juce::StringArray getMappedIds() const;

private:
    enum class Kind { continuous, toggle, choice, action };
    enum class Source { controller, note };

    int indexOf (const juce::String& paramId) const;
    juce::String idOf (int index) const;
    void handle (Source, int number, int value);
    void handleAsyncUpdate() override;
    void load();
    void save() const;

    juce::AudioProcessorValueTreeState& state;
    juce::File file;
    std::vector<juce::RangedAudioParameter*> parameters;
    std::vector<Action> actions;
    std::vector<Kind> kinds;
    std::array<int, 128> lastControllerValues {};
    std::array<std::atomic<int>, 128> controllers, notes;   // parameter index, or -1
    std::atomic<int> learning { -1 }, pendingProgram { -1 };
    std::atomic<bool> mappingsChanged { false };
};
