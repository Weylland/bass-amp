#include "PresetManager.h"
#include "FactoryPresets.h"
#include "../PedalChain.h"

namespace
{
    const juce::Identifier presetNameProperty { "presetName" };
    const juce::Identifier factoryPresetProperty { "factoryPreset" };
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

juce::File PresetManager::getUserPresetFile (const juce::String& name) const
{
    return userDirectory.getChildFile (juce::File::createLegalFileName (name.trim()) + fileExtension);
}

juce::File PresetManager::getCurrentUserPreset() const
{
    const auto file = getUserPresetFile (getCurrentPresetName());
    return getCurrentFactoryIndex() < 0 && getCurrentPresetName().isNotEmpty() && file.existsAsFile() ? file : juce::File();
}

juce::File PresetManager::renameUserPreset (const juce::File& file, const juce::String& newName)
{
    const auto target = getUserPresetFile (newName);

    if (juce::File::createLegalFileName (newName.trim()).isEmpty() || target.exists() || ! file.moveFileTo (target))
        return {};

    if (getCurrentFactoryIndex() < 0 && getCurrentPresetName() == file.getFileNameWithoutExtension())
        setCurrentPresetName (target.getFileNameWithoutExtension());

    return target;
}

bool PresetManager::deleteUserPreset (const juce::File& file)
{
    return file.isAChildOf (userDirectory) && file.moveToTrash();
}

bool PresetManager::exportPreset (const juce::File& zipFile) const
{
    auto tree = state.copyState();
    juce::ZipFile::Builder zip;

    for (const auto& property : { impulseResponseProperty, captureProperty })
    {
        const juce::File attached (tree.getProperty (property).toString());

        if (juce::File::isAbsolutePath (attached.getFullPathName()) && attached.existsAsFile())
        {
            zip.addFile (attached, 9, attached.getFileName());
            tree.setProperty (property, attached.getFileName(), nullptr);
        }
    }

    const auto text = tree.createXml()->toString();
    const auto name = juce::File::createLegalFileName (getCurrentPresetName().isNotEmpty() ? getCurrentPresetName() : juce::String ("Preset"));
    zip.addEntry (new juce::MemoryInputStream (text.toRawUTF8(), text.getNumBytesAsUTF8(), true),
                  9, name + fileExtension, juce::Time::getCurrentTime());

    zipFile.deleteFile();
    juce::FileOutputStream stream (zipFile);
    return stream.openedOk() && zip.writeToStream (stream, nullptr) && stream.getStatus().wasOk();
}

juce::File PresetManager::importPreset (const juce::File& source)
{
    if (! userDirectory.createDirectory())
        return {};

    if (source.hasFileExtension (fileExtension))
    {
        const auto xml = juce::XmlDocument::parse (source);

        if (xml == nullptr || ! xml->hasTagName (state.state.getType()))
            return {};

        const auto target = userDirectory.getNonexistentChildFile (source.getFileNameWithoutExtension(), fileExtension);
        return source.copyFileTo (target) ? target : juce::File();
    }

    juce::ZipFile zip (source);
    const juce::ZipFile::ZipEntry* presetEntry = nullptr;

    for (int i = 0; i < zip.getNumEntries(); ++i)
        if (zip.getEntry (i)->filename.endsWithIgnoreCase (fileExtension) && ! zip.getEntry (i)->filename.containsAnyOf ("/\\"))
            presetEntry = zip.getEntry (i);

    if (presetEntry == nullptr)
        return {};

    const std::unique_ptr<juce::InputStream> stream (zip.createStreamForEntry (*presetEntry));
    const auto xml = stream != nullptr ? juce::XmlDocument::parse (stream->readEntireStreamAsString()) : nullptr;

    if (xml == nullptr || ! xml->hasTagName (state.state.getType()))
        return {};

    const auto target = userDirectory.getNonexistentChildFile (juce::File::createLegalFileName (presetEntry->filename.upToLastOccurrenceOf (".", false, false)),
                                                               fileExtension);
    const auto filesDirectory = userDirectory.getChildFile ("Files").getChildFile (target.getFileNameWithoutExtension());
    auto tree = juce::ValueTree::fromXml (*xml);

    for (const auto& property : { impulseResponseProperty, captureProperty })
    {
        const auto attached = tree.getProperty (property).toString();

        if (attached.isEmpty() || juce::File::isAbsolutePath (attached))
            continue;

        for (int i = 0; i < zip.getNumEntries(); ++i)
        {
            if (zip.getEntry (i)->filename != attached || attached.containsAnyOf ("/\\:")
                || ! juce::StringArray { ".wav", ".aif", ".aiff", ".flac", ".nam" }.contains (juce::File (attached).getFileExtension(), true))
                continue;

            if (filesDirectory.createDirectory() && zip.uncompressEntry (i, filesDirectory).wasOk())
                tree.setProperty (property, filesDirectory.getChildFile (attached).getFullPathName(), nullptr);
        }
    }

    const auto written = tree.createXml();
    return written != nullptr && written->writeTo (target) ? target : juce::File();
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

    state.state.setProperty (factoryPresetProperty, index, nullptr);
    setCurrentPresetName (preset.name);
    markUnmodified();
}

bool PresetManager::loadUserPreset (const juce::File& file)
{
    const auto xml = juce::XmlDocument::parse (file);

    if (xml == nullptr || ! xml->hasTagName (state.state.getType()))
        return false;

    restoreState (juce::ValueTree::fromXml (*xml));
    state.state.removeProperty (factoryPresetProperty, nullptr);
    setCurrentPresetName (file.getFileNameWithoutExtension());
    markUnmodified();
    return true;
}

juce::File PresetManager::saveUserPreset (const juce::String& name)
{
    const auto legalName = juce::File::createLegalFileName (name.trim());

    if (legalName.isEmpty() || ! userDirectory.createDirectory())
        return {};

    const auto file = getUserPresetFile (legalName);
    state.state.removeProperty (factoryPresetProperty, nullptr);
    setCurrentPresetName (legalName);

    if (auto xml = state.copyState().createXml(); xml == nullptr || ! xml->writeTo (file))
        return {};

    markUnmodified();
    return file;
}

void PresetManager::restoreState (juce::ValueTree tree)
{
    for (auto* parameter : state.processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (parameter))
            if (! tree.getChildWithProperty ("id", ranged->getParameterID()).isValid())
                tree.appendChild (juce::ValueTree ("PARAM", { { "id", ranged->getParameterID() },
                                                              { "value", ranged->convertFrom0to1 (ranged->getDefaultValue()) } }),
                                  nullptr);

    state.replaceState (tree);
}

int PresetManager::getCurrentFactoryIndex() const
{
    return state.state.getProperty (factoryPresetProperty, -1);
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
