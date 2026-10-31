#include "PresetBar.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "../i18n/Language.h"

PresetBar::PresetBar (PresetManager& presetManager)
    : presets (presetManager)
{
    presetBox.setTextWhenNothingSelected ("Preset");
    presetBox.onChange = [this] { confirmAndLoad (presetBox.getSelectedId()); };

    previousButton.onClick = [this] { step (-1); };
    nextButton.onClick = [this] { step (1); };
    saveButton.onClick = [this] { askNameAndSave(); };

    presetBox.setTooltip (Help::get ("ui.presetBox"));
    previousButton.setTooltip (Help::get ("ui.previousPreset"));
    nextButton.setTooltip (Help::get ("ui.nextPreset"));
    saveButton.setTooltip (Help::get ("ui.savePreset"));

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
    presetBox.addSectionHeading (Language::tr ("Factory"));

    for (int i = 0; i < numFactory; ++i)
        presetBox.addItem (presets.getFactoryPresetName (i), i + 1);

    if (! userPresets.isEmpty())
    {
        presetBox.addSeparator();
        presetBox.addSectionHeading (Language::tr ("User"));

        for (int i = 0; i < userPresets.size(); ++i)
            presetBox.addItem (userPresets[i].getFileNameWithoutExtension(), numFactory + i + 1);
    }

    shownName = presets.getCurrentPresetName();
    const auto factoryIndex = presets.getCurrentFactoryIndex();
    auto selectedId = factoryIndex >= 0 ? factoryIndex + 1 : 0;

    for (int i = 0; selectedId == 0 && i < userPresets.size(); ++i)
        if (userPresets[i].getFileNameWithoutExtension() == shownName)
            selectedId = numFactory + i + 1;

    loadedItemId = selectedId;

    if (selectedId > 0)
        presetBox.setSelectedId (selectedId, juce::dontSendNotification);
    else
        presetBox.setText (shownName, juce::dontSendNotification);
}

void PresetBar::timerCallback()
{
    if (presets.getCurrentPresetName() != shownName)
        refreshList();

    if (const auto nowModified = presets.isModified(); nowModified != modified)
    {
        modified = nowModified;
        repaint();
    }
}

void PresetBar::confirmAndLoad (int itemId)
{
    if (itemId == loadedItemId && ! modified)
        return;

    if (! modified)
    {
        loadItem (itemId);
        return;
    }

    auto* window = new juce::AlertWindow (Language::tr ("Unsaved changes"),
                                          Language::tr ("Your changes to this sound will be lost.") + "\n\n" + shownName,
                                          juce::MessageBoxIconType::NoIcon, this);
    window->addButton (Language::tr ("Load anyway"), 1, juce::KeyPress (juce::KeyPress::returnKey));
    window->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));

    window->enterModalState (true, juce::ModalCallbackFunction::create ([safeThis = SafePointer<PresetBar> (this), itemId] (int result)
    {
        if (safeThis == nullptr)
            return;

        if (result == 1)
            safeThis->loadItem (itemId);
        else if (safeThis->loadedItemId > 0)
            safeThis->presetBox.setSelectedId (safeThis->loadedItemId, juce::dontSendNotification);
        else
            safeThis->presetBox.setText (safeThis->shownName, juce::dontSendNotification);
    }), true);
}

void PresetBar::loadItem (int itemId)
{
    const auto numFactory = presets.getNumFactoryPresets();

    if (itemId >= 1 && itemId <= numFactory)
        presets.loadFactoryPreset (itemId - 1);
    else if (juce::isPositiveAndBelow (itemId - numFactory - 1, userPresets.size()))
        presets.loadUserPreset (userPresets[itemId - numFactory - 1]);

    shownName = presets.getCurrentPresetName();
    loadedItemId = itemId;
    modified = false;
    repaint();
}

void PresetBar::step (int delta)
{
    const auto numItems = presets.getNumFactoryPresets() + userPresets.size();
    const auto current = juce::jmax (1, presetBox.getSelectedId());
    presetBox.setSelectedId ((current - 1 + delta + numItems) % numItems + 1, juce::sendNotificationSync);
}

void PresetBar::askNameAndSave()
{
    auto* window = new juce::AlertWindow (Language::tr ("Save preset"), Language::tr ("Preset name:"), juce::MessageBoxIconType::NoIcon, this);
    window->addTextEditor ("name", presets.getCurrentFactoryIndex() >= 0 ? presets.getCurrentPresetName() + " (" + Language::tr ("copy") + ")"
                                                                         : presets.getCurrentPresetName());
    window->addButton (Language::tr ("Save"), 1, juce::KeyPress (juce::KeyPress::returnKey));
    window->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));

    window->enterModalState (true, juce::ModalCallbackFunction::create ([safeThis = SafePointer<PresetBar> (this), window] (int result)
    {
        if (safeThis != nullptr && result == 1
            && safeThis->presets.saveUserPreset (window->getTextEditorContents ("name")).existsAsFile())
            safeThis->refreshList();
    }), true);
}

void PresetBar::paintOverChildren (juce::Graphics& g)
{
    if (! modified)
        return;

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (20.0f));
    g.drawText ("*", presetBox.getBounds().withTrimmedRight (22).removeFromRight (16), juce::Justification::centred);
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
