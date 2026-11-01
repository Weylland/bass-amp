#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "FactoryPresets.h"

// Presets for one unit of the rig: "amp", "eq" or a pedal key. Loading one sets every parameter
// of the unit, back to its default when the preset doesn't list it, and switches the unit on.
// Nothing outside the unit changes.
const std::vector<FactoryPreset>& getModulePresets (const juce::String& module);
void applyModulePreset (juce::AudioProcessorValueTreeState&, const juce::String& module, const FactoryPreset&);
bool isModulePresetActive (const juce::AudioProcessorValueTreeState&, const juce::String& module, const FactoryPreset&);

// The user's own presets of a unit: XML files of all its parameters, in <root>/<module>/<name>.xml.
// The amp's also keep its IR and capture files. Message thread only.
juce::File getDefaultModulePresetDirectory();
juce::Array<juce::File> getUserModulePresets (const juce::String& module, const juce::File& root = getDefaultModulePresetDirectory());
juce::File getUserModulePresetFile (const juce::String& module, const juce::String& name,
                                    const juce::File& root = getDefaultModulePresetDirectory());
juce::File saveUserModulePreset (const juce::AudioProcessorValueTreeState&, const juce::String& module, const juce::String& name,
                                 const juce::File& root = getDefaultModulePresetDirectory());
bool loadUserModulePreset (juce::AudioProcessorValueTreeState&, const juce::String& module, const juce::File&);
bool isUserModulePresetActive (const juce::AudioProcessorValueTreeState&, const juce::String& module, const juce::File&);
