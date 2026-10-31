#include "PresetManager.h"
#include "FactoryPresets.h"
#include "../PedalChain.h"

namespace
{
    const juce::Identifier presetNameProperty { "presetName" };
}

PresetManager::PresetManager (juce::AudioProcessorValueTreeState& stateToUse, juce::File directory)
    : state (stateToUse), userDirectory (std::move (directory))
{
}

juce::File PresetManager::getDefaultUserDirectory()
{
    return juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("Bass Amp").getChildFile ("Presets");
}

int PresetManager::getNumFactoryPresets() const
{
    return (int) getFactoryPresets().size();
}

juce::String PresetManager::getFactoryPresetName (int index) const
{
    return juce::isPositiveAndBelow (index, getNumFactoryPresets()) ? juce::String (getFactoryPresets()[(size_t) index].name)
                                                                    : juce::String();
}

juce::Array<juce::File> PresetManager::getUserPresets() const
{
    auto files = userDirectory.findChildFiles (juce::File::findFiles, false, juce::String ("*") + fileExtension);
    files.sort();
    return files;
}

void PresetManager::loadFactoryPreset (int index)
{
    if (! juce::isPositiveAndBelow (index, getNumFactoryPresets()))
        return;

    const auto& preset = getFactoryPresets()[(size_t) index];

    state.state.removeProperty (PedalChain::property, nullptr);

    for (auto* parameter : state.processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (parameter))
            ranged->setValueNotifyingHost (ranged->getDefaultValue());

    for (const auto& [paramId, value] : preset.values)
        if (auto* parameter = state.getParameter (paramId))
            parameter->setValueNotifyingHost (parameter->convertTo0to1 (value));

    currentFactoryIndex = index;
    setCurrentPresetName (preset.name);
    markUnmodified();
}

bool PresetManager::loadUserPreset (const juce::File& file)
{
    const auto xml = juce::XmlDocument::parse (file);

    if (xml == nullptr || ! xml->hasTagName (state.state.getType()))
        return false;

    state.replaceState (juce::ValueTree::fromXml (*xml));
    currentFactoryIndex = -1;
    setCurrentPresetName (file.getFileNameWithoutExtension());
    markUnmodified();
    return true;
}

juce::File PresetManager::saveUserPreset (const juce::String& name)
{
    const auto legalName = juce::File::createLegalFileName (name.trim());

    if (legalName.isEmpty() || ! userDirectory.createDirectory())
        return {};

    const auto file = userDirectory.getChildFile (legalName + fileExtension);
    setCurrentPresetName (legalName);

    if (auto xml = state.copyState().createXml(); xml == nullptr || ! xml->writeTo (file))
        return {};

    currentFactoryIndex = -1;
    markUnmodified();
    return file;
}

bool PresetManager::isModified()
{
    const auto values = currentValues();
    const auto chain = state.state.getProperty (PedalChain::property).toString();

    if (referencePending.exchange (false))
    {
        reference = values;
        referenceChain = chain;
        return false;
    }

    if (chain != referenceChain || values.size() != reference.size())
        return true;

    for (size_t i = 0; i < values.size(); ++i)
        if (std::abs (values[i] - reference[i]) > 1.0e-4f)
            return true;

    return false;
}

std::vector<float> PresetManager::currentValues() const
{
    std::vector<float> values;

    for (auto* parameter : state.processor.getParameters())
        values.push_back (parameter->getValue());

    return values;
}

juce::String PresetManager::getCurrentPresetName() const
{
    return state.state.getProperty (presetNameProperty).toString();
}

void PresetManager::setCurrentPresetName (const juce::String& name)
{
    state.state.setProperty (presetNameProperty, name, nullptr);
}
