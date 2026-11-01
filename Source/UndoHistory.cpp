#include "UndoHistory.h"

UndoHistory::UndoHistory (juce::AudioProcessorValueTreeState& stateToUse, std::function<juce::StringArray()> midiDriven)
    : state (stateToUse), midiDrivenIds (std::move (midiDriven))
{
    for (auto* parameter : state.processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (parameter))
            parameters.push_back (ranged);

    clear();
}

UndoHistory::Snapshot UndoHistory::capture() const
{
    Snapshot snapshot;

    for (int i = 0; i < state.state.getNumProperties(); ++i)
        snapshot.properties.set (state.state.getPropertyName (i), state.state.getProperty (state.state.getPropertyName (i)));

    for (auto* parameter : parameters)
        snapshot.values.push_back (parameter->getValue());

    return snapshot;
}

bool UndoHistory::same (const Snapshot& a, const Snapshot& b, const juce::StringArray& ignored) const
{
    if (a.properties != b.properties)
        return false;

    for (size_t i = 0; i < parameters.size(); ++i)
        if (std::abs (a.values[i] - b.values[i]) > 1.0e-6f && ! ignored.contains (parameters[i]->getParameterID()))
            return false;

    return true;
}

void UndoHistory::restore (const Snapshot& snapshot)
{
    const auto ignored = midiDrivenIds();

    for (size_t i = 0; i < parameters.size(); ++i)
    {
        auto* parameter = parameters[i];

        if (std::abs (parameter->getValue() - snapshot.values[i]) > 1.0e-6f && ! ignored.contains (parameter->getParameterID()))
        {
            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost (snapshot.values[i]);
            parameter->endChangeGesture();
        }
    }

    for (int i = state.state.getNumProperties(); --i >= 0;)
        if (const auto name = state.state.getPropertyName (i); ! snapshot.properties.contains (name))
            state.state.removeProperty (name, nullptr);

    for (const auto& property : snapshot.properties)
        state.state.setProperty (property.name, property.value, nullptr);
}

void UndoHistory::checkpoint()
{
    auto now = capture();

    if (same (now, committed, midiDrivenIds()))
        return;

    undoSteps.push_back (std::move (committed));
    redoSteps.clear();
    committed = std::move (now);

    if (undoSteps.size() > maxSteps)
        undoSteps.erase (undoSteps.begin());
}

bool UndoHistory::undo()
{
    checkpoint();

    if (undoSteps.empty())
        return false;

    redoSteps.push_back (std::move (committed));
    restore (undoSteps.back());
    undoSteps.pop_back();
    committed = capture();
    return true;
}

bool UndoHistory::redo()
{
    checkpoint();

    if (redoSteps.empty())
        return false;

    undoSteps.push_back (std::move (committed));
    restore (redoSteps.back());
    redoSteps.pop_back();
    committed = capture();
    return true;
}

void UndoHistory::clear()
{
    undoSteps.clear();
    redoSteps.clear();
    committed = capture();
}
