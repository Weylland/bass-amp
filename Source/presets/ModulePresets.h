#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "FactoryPresets.h"

// Presets for one unit of the rig: "amp", "eq" or a pedal key. Loading one sets every parameter
// of the unit, back to its default when the preset doesn't list it, and switches the unit on.
// Nothing outside the unit changes.
const std::vector<FactoryPreset>& getModulePresets (const juce::String& module);
void applyModulePreset (juce::AudioProcessorValueTreeState&, const juce::String& module, const FactoryPreset&);
bool isModulePresetActive (const juce::AudioProcessorValueTreeState&, const juce::String& module, const FactoryPreset&);
