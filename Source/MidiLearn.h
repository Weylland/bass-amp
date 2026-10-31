#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// MIDI control. A CC drives any parameter (switches flip at 64); a note toggles a switch or
// steps through a choice; a program change loads a factory preset. Mappings are learnt from
// the next CC or note after learn(), and saved in a file shared by every session rather than
// in presets. Without a saved map, CC 11 (expression pedal) drives the wah.
class MidiLearn : private juce::AsyncUpdater
{
public:
    MidiLearn (juce::AudioProcessorValueTreeState&, juce::File mapFile);
    ~MidiLearn() override;

    static juce::File getDefaultFile();

    // Called on the message thread for a received program change.
    std::function<void (int)> onProgramChange;

    // Audio thread.
    void process (const juce::MidiBuffer&);

    // Message thread.
    void learn (const juce::String& paramId);
    void cancelLearning();
    juce::String getLearningParameter() const;
    void forget (const juce::String& paramId);
    juce::String describe (const juce::String& paramId) const;

private:
    enum class Kind { continuous, toggle, choice };
    enum class Source { controller, note };

    int indexOf (const juce::String& paramId) const;
    void handle (Source, int number, int value);
    void handleAsyncUpdate() override;
    void load();
    void save() const;

    juce::AudioProcessorValueTreeState& state;
    juce::File file;
    std::vector<juce::RangedAudioParameter*> parameters;
    std::vector<Kind> kinds;
    std::array<std::atomic<int>, 128> controllers, notes;   // parameter index, or -1
    std::atomic<int> learning { -1 }, pendingProgram { -1 };
    std::atomic<bool> mappingsChanged { false };
};
