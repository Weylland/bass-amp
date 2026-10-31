#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Pops up the presets of one unit ("amp", "eq" or a pedal key) under the given component,
// with the preset that matches the current settings ticked.
void showModulePresetMenu (juce::AudioProcessorValueTreeState&, const juce::String& module, juce::Component& target);
