#include "PresetBar.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "Message.h"
#include "../i18n/Language.h"

PresetBar::PresetBar (PresetManager& presetManager)
    : presets (presetManager)
{
    presetBox.setTextWhenNothingSelected ("Preset");
    presetBox.onChange = [this] { confirmAndLoad (presetBox.getSelectedId()); };

    previousButton.onClick = [this] { step (-1); };
    nextButton.onClick = [this] { step (1); };
    saveButton.onClick = [this] { askNameAndSave(); };
    menuButton.setButtonText ({});
    menuButton.onClick = [this] { showMenu(); };

    presetBox.setTooltip (Help::get ("ui.presetBox"));
    previousButton.setTooltip (Help::get ("ui.previousPreset"));
    nextButton.setTooltip (Help::get ("ui.nextPreset"));
    saveButton.setTooltip (Help::get ("ui.savePreset"));
    menuButton.setTooltip (Help::get ("ui.presetMenu"));

    for (auto* component : std::initializer_list<juce::Component*> { &presetBox, &previousButton, &nextButton, &menuButton, &saveButton })
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
        if (safeThis == nullptr || result != 1)
            return;

        const auto name = juce::File::createLegalFileName (window->getTextEditorContents ("name").trim());

        if (name.isEmpty())
        {
            safeThis->askNameAndSave();
            return;
        }

        if (! safeThis->presets.getUserPresetFile (name).existsAsFile())
        {
            safeThis->save (name);
            return;
        }

        auto* confirm = new juce::AlertWindow (Language::tr ("Replace preset?"),
                                               name + "\n\n" + Language::tr ("A preset with this name already exists."),
                                               juce::MessageBoxIconType::NoIcon, safeThis.getComponent());
        confirm->addButton (Language::tr ("Replace"), 1, juce::KeyPress (juce::KeyPress::returnKey));
        confirm->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));
        confirm->enterModalState (true, juce::ModalCallbackFunction::create ([safeThis, name] (int answer)
        {
            if (safeThis != nullptr && answer == 1)
                safeThis->save (name);
        }), true);
    }), true);
}

void PresetBar::save (const juce::String& name)
{
    const auto file = presets.saveUserPreset (name);

    if (file.existsAsFile())
        refreshList();
    else
        Message::show ("Error whilst saving", "Couldn't write to the specified file!", presets.getUserPresetFile (name).getFullPathName());
}

void PresetBar::showMenu()
{
    const auto current = presets.getCurrentUserPreset();
    juce::PopupMenu menu;
    menu.addItem (Language::tr ("Rename..."), current.existsAsFile(), false, [this, current] { askNameAndRename (current); });
    menu.addItem (Language::tr ("Delete..."), current.existsAsFile(), false, [this, current] { confirmAndDelete (current); });
    menu.addSeparator();
    menu.addItem (Language::tr ("Import..."), [this] { chooseAndImport(); });
    menu.addItem (Language::tr ("Export..."), [this] { chooseAndExport(); });
    menu.addSeparator();
    menu.addItem (Language::tr ("Open presets folder"), [this]
    {
        const auto directory = presets.getUserDirectory();

        if (directory.createDirectory())
            directory.startAsProcess();
    });

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (menuButton));
}

void PresetBar::askNameAndRename (const juce::File& file)
{
    auto* window = new juce::AlertWindow (Language::tr ("Rename preset"), Language::tr ("Preset name:"), juce::MessageBoxIconType::NoIcon, this);
    window->addTextEditor ("name", file.getFileNameWithoutExtension());
    window->addButton (Language::tr ("Rename"), 1, juce::KeyPress (juce::KeyPress::returnKey));
    window->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));

    window->enterModalState (true, juce::ModalCallbackFunction::create ([safeThis = SafePointer<PresetBar> (this), window, file] (int result)
    {
        if (safeThis == nullptr || result != 1)
            return;

        const auto name = window->getTextEditorContents ("name");

        if (safeThis->presets.renameUserPreset (file, name).existsAsFile())
            safeThis->refreshList();
        else if (safeThis->presets.getUserPresetFile (name).exists())
            Message::show ("Replace preset?", "A preset with this name already exists.", name);
        else
            Message::show ("Error whilst saving", "Couldn't write to the specified file!", file.getFullPathName());
    }), true);
}

void PresetBar::confirmAndDelete (const juce::File& file)
{
    auto* window = new juce::AlertWindow (Language::tr ("Delete preset?"), file.getFileNameWithoutExtension(), juce::MessageBoxIconType::NoIcon, this);
    window->addButton (Language::tr ("Delete"), 1);
    window->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));

    window->enterModalState (true, juce::ModalCallbackFunction::create ([safeThis = SafePointer<PresetBar> (this), file] (int result)
    {
        if (safeThis != nullptr && result == 1 && safeThis->presets.deleteUserPreset (file))
            safeThis->refreshList();
    }), true);
}

void PresetBar::chooseAndImport()
{
    fileChooser = std::make_unique<juce::FileChooser> (Language::tr ("Import a preset"), juce::File::getSpecialLocation (juce::File::userHomeDirectory).getChildFile ("Downloads"),
                                                       juce::String ("*") + PresetManager::fileExtension + ";*.zip");

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [safeThis = SafePointer<PresetBar> (this)] (const juce::FileChooser& chooser)
    {
        const auto source = chooser.getResult();

        if (safeThis == nullptr || ! source.existsAsFile())
            return;

        const auto imported = safeThis->presets.importPreset (source);

        if (! imported.existsAsFile())
        {
            Message::show ("Error whilst loading", "This file isn't a Bass Amp preset.", source.getFullPathName());
            return;
        }

        safeThis->refreshList();

        if (const auto index = safeThis->userPresets.indexOf (imported); index >= 0)
        {
            const auto itemId = safeThis->presets.getNumFactoryPresets() + index + 1;
            safeThis->presetBox.setSelectedId (itemId, juce::dontSendNotification);
            safeThis->confirmAndLoad (itemId);
        }
    });
}

void PresetBar::chooseAndExport()
{
    const auto name = juce::File::createLegalFileName (presets.getCurrentPresetName().isNotEmpty() ? presets.getCurrentPresetName() : juce::String ("Preset"));
    fileChooser = std::make_unique<juce::FileChooser> (Language::tr ("Export this sound"),
                                                       juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile (name + ".zip"),
                                                       "*.zip");

    fileChooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles
                                  | juce::FileBrowserComponent::warnAboutOverwriting,
                              [safeThis = SafePointer<PresetBar> (this)] (const juce::FileChooser& chooser)
    {
        auto target = chooser.getResult();

        if (safeThis == nullptr || target == juce::File())
            return;

        target = target.withFileExtension (".zip");

        if (! safeThis->presets.exportPreset (target))
            Message::show ("Error whilst saving", "Couldn't write to the specified file!", target.getFullPathName());
        else
            target.revealToUser();
    });
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
    area.removeFromRight (4);
    menuButton.setBounds (area.removeFromRight (30));
    area.removeFromRight (8);
    nextButton.setBounds (area.removeFromRight (28));
    area.removeFromRight (4);
    previousButton.setBounds (area.removeFromLeft (28));
    area.removeFromLeft (4);
    presetBox.setBounds (area);
}
