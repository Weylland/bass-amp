#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Undo and redo of the whole sound: parameters, pedal order, loaded files and preset name. A
// step ends at each click in the editor, so a knob gesture, a preset load or a pedal move is
// one step. Controls driven by MIDI are left out: the controller sets them anyway, and an
// expression pedal would otherwise fill the history. Message thread only.
class UndoHistory
{
public:
    UndoHistory (juce::AudioProcessorValueTreeState&, std::function<juce::StringArray()> midiDrivenIds);

    void checkpoint();
    bool undo();
    bool redo();
    void clear();

private:
    struct Snapshot
    {
        std::vector<float> values;
        juce::NamedValueSet properties;
    };

    Snapshot capture() const;
    bool same (const Snapshot&, const Snapshot&, const juce::StringArray& ignored) const;
    void restore (const Snapshot&);

    static constexpr size_t maxSteps = 100;

    juce::AudioProcessorValueTreeState& state;
    std::function<juce::StringArray()> midiDrivenIds;
    std::vector<juce::RangedAudioParameter*> parameters;
    std::vector<Snapshot> undoSteps, redoSteps;
    Snapshot committed;
};
