#include "ModulePresetMenu.h"
#include "Message.h"
#include "../presets/ModulePresets.h"

namespace
{
    using SafeTarget = juce::Component::SafePointer<juce::Component>;

    void save (const juce::AudioProcessorValueTreeState& state, const juce::String& module, const juce::String& name)
    {
        if (! saveUserModulePreset (state, module, name).existsAsFile())
            Message::show ("Error whilst saving", "Couldn't write to the specified file!", getUserModulePresetFile (module, name).getFullPathName());
    }

    void askNameAndSave (juce::AudioProcessorValueTreeState& state, const juce::String& module, SafeTarget target)
    {
        auto* window = new juce::AlertWindow (Language::tr ("Save preset"), Language::tr ("Preset name:"), juce::MessageBoxIconType::NoIcon,
                                              target.getComponent());
        window->addTextEditor ("name", {});
        window->addButton (Language::tr ("Save"), 1, juce::KeyPress (juce::KeyPress::returnKey));
        window->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));

        window->enterModalState (true, juce::ModalCallbackFunction::create ([&state, module, target, window] (int result)
        {
            if (result != 1)
                return;

            const auto name = juce::File::createLegalFileName (window->getTextEditorContents ("name").trim());

            if (name.isEmpty())
            {
                askNameAndSave (state, module, target);
                return;
            }

            if (! getUserModulePresetFile (module, name).existsAsFile())
            {
                save (state, module, name);
                return;
            }

            auto* confirm = new juce::AlertWindow (Language::tr ("Replace preset?"),
                                                   name + "\n\n" + Language::tr ("A preset with this name already exists."),
                                                   juce::MessageBoxIconType::NoIcon, target.getComponent());
            confirm->addButton (Language::tr ("Replace"), 1, juce::KeyPress (juce::KeyPress::returnKey));
            confirm->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));
            confirm->enterModalState (true, juce::ModalCallbackFunction::create ([&state, module, name] (int answer)
            {
                if (answer == 1)
                    save (state, module, name);
            }), true);
        }), true);
    }

    void confirmAndDelete (const juce::File& file, SafeTarget target)
    {
        auto* window = new juce::AlertWindow (Language::tr ("Delete preset?"), file.getFileNameWithoutExtension(),
                                              juce::MessageBoxIconType::NoIcon, target.getComponent());
        window->addButton (Language::tr ("Delete"), 1);
        window->addButton (Language::tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));
        window->enterModalState (true, juce::ModalCallbackFunction::create ([file] (int result)
        {
            if (result == 1)
                file.moveToTrash();
        }), true);
    }
}

void showModulePresetMenu (juce::AudioProcessorValueTreeState& state, const juce::String& module, juce::Component& target)
{
    juce::PopupMenu menu;

    for (const auto& preset : getModulePresets (module))
        menu.addItem (preset.name, true, isModulePresetActive (state, module, preset),
                      [&state, module, &preset] { applyModulePreset (state, module, preset); });

    const auto mine = getUserModulePresets (module);
    const auto safeTarget = SafeTarget (&target);
    juce::PopupMenu deleting;

    if (! mine.isEmpty())
    {
        menu.addSectionHeader (Language::tr ("User"));

        for (const auto& file : mine)
        {
            menu.addItem (file.getFileNameWithoutExtension(), true, isUserModulePresetActive (state, module, file),
                          [&state, module, file] { loadUserModulePreset (state, module, file); });
            deleting.addItem (file.getFileNameWithoutExtension(), [file, safeTarget] { confirmAndDelete (file, safeTarget); });
        }
    }

    menu.addSeparator();
    menu.addItem (Language::tr ("Save..."), [&state, module, safeTarget] { askNameAndSave (state, module, safeTarget); });
    menu.addSubMenu (Language::tr ("Delete"), deleting, ! mine.isEmpty());

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&target));
}
