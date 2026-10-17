#include "PresetBar.h"

PresetBar::PresetBar (PresetManager& presetManager)
    : presets (presetManager)
{
    presetBox.setTextWhenNothingSelected ("Preset");
    presetBox.onChange = [this] { loadItem (presetBox.getSelectedId()); };

    previousButton.onClick = [this] { step (-1); };
    nextButton.onClick = [this] { step (1); };
    saveButton.onClick = [this] { askNameAndSave(); };

    for (auto* component : std::initializer_list<juce::Component*> { &presetBox, &previousButton, &nextButton, &saveButton })
        addAndMakeVisible (component);

    refreshList();
    startTimerHz (4);
}

PresetBar::~PresetBar()
{
    stopTimer();
}

void PresetBar::refreshList()
{
    userPresets = presets.getUserPresets();
    presetBox.clear (juce::dontSendNotification);

    const auto numFactory = presets.getNumFactoryPresets();
    presetBox.addSectionHeading ("Factory");

    for (int i = 0; i < numFactory; ++i)
        presetBox.addItem (presets.getFactoryPresetName (i), i + 1);

    if (! userPresets.isEmpty())
    {
        presetBox.addSeparator();
        presetBox.addSectionHeading ("User");

        for (int i = 0; i < userPresets.size(); ++i)
            presetBox.addItem (userPresets[i].getFileNameWithoutExtension(), numFactory + i + 1);
    }

    shownName = presets.getCurrentPresetName();
    const auto factoryIndex = presets.getCurrentFactoryIndex();
    auto selectedId = factoryIndex >= 0 ? factoryIndex + 1 : 0;

    for (int i = 0; selectedId == 0 && i < userPresets.size(); ++i)
        if (userPresets[i].getFileNameWithoutExtension() == shownName)
            selectedId = numFactory + i + 1;

    if (selectedId > 0)
        presetBox.setSelectedId (selectedId, juce::dontSendNotification);
    else
        presetBox.setText (shownName, juce::dontSendNotification);
}

void PresetBar::timerCallback()
{
    if (presets.getCurrentPresetName() != shownName)
        refreshList();
}

void PresetBar::loadItem (int itemId)
{
    const auto numFactory = presets.getNumFactoryPresets();

    if (itemId >= 1 && itemId <= numFactory)
        presets.loadFactoryPreset (itemId - 1);
    else if (juce::isPositiveAndBelow (itemId - numFactory - 1, userPresets.size()))
        presets.loadUserPreset (userPresets[itemId - numFactory - 1]);

    shownName = presets.getCurrentPresetName();
}

void PresetBar::step (int delta)
{
    const auto numItems = presets.getNumFactoryPresets() + userPresets.size();
    const auto current = juce::jmax (1, presetBox.getSelectedId());
    presetBox.setSelectedId ((current - 1 + delta + numItems) % numItems + 1, juce::sendNotificationSync);
}

void PresetBar::askNameAndSave()
{
    auto* window = new juce::AlertWindow ("Save preset", "Preset name:", juce::MessageBoxIconType::NoIcon, this);
    window->addTextEditor ("name", presets.getCurrentFactoryIndex() >= 0 ? presets.getCurrentPresetName() + " (copy)"
                                                                         : presets.getCurrentPresetName());
    window->addButton ("Save", 1, juce::KeyPress (juce::KeyPress::returnKey));
    window->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));

    window->enterModalState (true, juce::ModalCallbackFunction::create ([safeThis = SafePointer<PresetBar> (this), window] (int result)
    {
        if (safeThis != nullptr && result == 1
            && safeThis->presets.saveUserPreset (window->getTextEditorContents ("name")).existsAsFile())
            safeThis->refreshList();
    }), true);
}

void PresetBar::resized()
{
    auto area = getLocalBounds();
    saveButton.setBounds (area.removeFromRight (64));
    area.removeFromRight (8);
    nextButton.setBounds (area.removeFromRight (28));
    area.removeFromRight (4);
    previousButton.setBounds (area.removeFromLeft (28));
    area.removeFromLeft (4);
    presetBox.setBounds (area);
}
