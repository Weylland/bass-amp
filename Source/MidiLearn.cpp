#include "MidiLearn.h"
#include "ParameterIds.h"

namespace
{
    constexpr int expressionController = 11;
}

MidiLearn::MidiLearn (juce::AudioProcessorValueTreeState& stateToUse, juce::File mapFile)
    : state (stateToUse), file (std::move (mapFile))
{
    for (auto* parameter : state.processor.getParameters())
    {
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (parameter))
        {
            parameters.push_back (ranged);
            kinds.push_back (dynamic_cast<juce::AudioParameterBool*> (ranged) != nullptr     ? Kind::toggle
                             : dynamic_cast<juce::AudioParameterChoice*> (ranged) != nullptr ? Kind::choice
                                                                                             : Kind::continuous);
        }
    }

    load();
}

MidiLearn::~MidiLearn()
{
    cancelPendingUpdate();
}

juce::File MidiLearn::getDefaultFile()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("Bass Amp").getChildFile ("MidiMap.xml");
}

int MidiLearn::indexOf (const juce::String& paramId) const
{
    for (size_t i = 0; i < parameters.size(); ++i)
        if (parameters[i]->getParameterID() == paramId)
            return (int) i;

    return -1;
}

void MidiLearn::process (const juce::MidiBuffer& midi)
{
    for (const auto metadata : midi)
    {
        const auto message = metadata.getMessage();

        if (message.isController())
            handle (Source::controller, message.getControllerNumber(), message.getControllerValue());
        else if (message.isNoteOn())
            handle (Source::note, message.getNoteNumber(), message.getVelocity());
        else if (message.isProgramChange())
        {
            pendingProgram.store (message.getProgramChangeNumber());
            triggerAsyncUpdate();
        }
    }
}

void MidiLearn::handle (Source source, int number, int value)
{
    auto& slots = source == Source::controller ? controllers : notes;

    if (const auto learnt = learning.load(); learnt >= 0)
    {
        // A note can only flip a switch or step a choice; keep waiting for a CC otherwise.
        if (source == Source::controller || kinds[(size_t) learnt] != Kind::continuous)
        {
            for (auto* list : { &controllers, &notes })
                for (auto& slot : *list)
                    if (slot.load() == learnt)
                        slot.store (-1);

            slots[(size_t) number].store (learnt);
            learning.store (-1);
            mappingsChanged.store (true);
            triggerAsyncUpdate();
        }
    }

    const auto index = slots[(size_t) number].load();

    if (index < 0)
        return;

    auto* parameter = parameters[(size_t) index];
    const auto kind = kinds[(size_t) index];

    if (source == Source::controller)
    {
        parameter->setValueNotifyingHost (kind == Kind::toggle ? (value >= 64 ? 1.0f : 0.0f) : (float) value / 127.0f);
    }
    else if (kind == Kind::toggle)
    {
        parameter->setValueNotifyingHost (parameter->getValue() > 0.5f ? 0.0f : 1.0f);
    }
    else if (kind == Kind::choice)
    {
        const auto steps = juce::jmax (2, parameter->getNumSteps());
        const auto current = juce::roundToInt (parameter->getValue() * (float) (steps - 1));
        parameter->setValueNotifyingHost ((float) ((current + 1) % steps) / (float) (steps - 1));
    }
}

void MidiLearn::handleAsyncUpdate()
{
    if (mappingsChanged.exchange (false))
        save();

    if (const auto program = pendingProgram.exchange (-1); program >= 0 && onProgramChange != nullptr)
        onProgramChange (program);
}

void MidiLearn::learn (const juce::String& paramId)
{
    learning.store (indexOf (paramId));
}

void MidiLearn::cancelLearning()
{
    learning.store (-1);
}

juce::String MidiLearn::getLearningParameter() const
{
    const auto index = learning.load();
    return index >= 0 ? parameters[(size_t) index]->getParameterID() : juce::String();
}

void MidiLearn::forget (const juce::String& paramId)
{
    const auto index = indexOf (paramId);

    for (auto* list : { &controllers, &notes })
        for (auto& slot : *list)
            if (index >= 0 && slot.load() == index)
                slot.store (-1);

    save();
}

juce::String MidiLearn::describe (const juce::String& paramId) const
{
    const auto index = indexOf (paramId);

    for (int number = 0; number < 128 && index >= 0; ++number)
    {
        if (controllers[(size_t) number].load() == index)
            return "CC " + juce::String (number);

        if (notes[(size_t) number].load() == index)
            return "Note " + juce::MidiMessage::getMidiNoteName (number, true, true, 3);
    }

    return {};
}

void MidiLearn::load()
{
    for (auto* list : { &controllers, &notes })
        for (auto& slot : *list)
            slot.store (-1);

    const auto xml = juce::XmlDocument::parse (file);

    if (xml == nullptr)
    {
        controllers[(size_t) expressionController].store (indexOf (ParamIds::wahPedal));
        return;
    }

    for (const auto* mapping : xml->getChildWithTagNameIterator ("MAP"))
    {
        const auto number = mapping->getIntAttribute ("number", -1);
        const auto index = indexOf (mapping->getStringAttribute ("param"));

        if (juce::isPositiveAndBelow (number, 128) && index >= 0)
            (mapping->getStringAttribute ("type") == "note" ? notes : controllers)[(size_t) number].store (index);
    }
}

void MidiLearn::save() const
{
    juce::XmlElement xml ("MIDIMAP");

    for (int number = 0; number < 128; ++number)
    {
        for (const auto& [type, list] : { std::pair { "cc", &controllers }, std::pair { "note", &notes } })
        {
            if (const auto index = (*list)[(size_t) number].load(); index >= 0)
            {
                auto* mapping = xml.createNewChildElement ("MAP");
                mapping->setAttribute ("type", type);
                mapping->setAttribute ("number", number);
                mapping->setAttribute ("param", parameters[(size_t) index]->getParameterID());
            }
        }
    }

    if (file.getParentDirectory().createDirectory())
        xml.writeTo (file);
}
