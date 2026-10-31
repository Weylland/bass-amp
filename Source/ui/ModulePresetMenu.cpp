#include "ModulePresetMenu.h"
#include "../presets/ModulePresets.h"

void showModulePresetMenu (juce::AudioProcessorValueTreeState& state, const juce::String& module, juce::Component& target)
{
    juce::PopupMenu menu;

    for (const auto& preset : getModulePresets (module))
        menu.addItem (preset.name, true, isModulePresetActive (state, module, preset),
                      [&state, module, &preset] { applyModulePreset (state, module, preset); });

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&target));
}
