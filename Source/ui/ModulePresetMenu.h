#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Pops up the presets of one unit ("amp", "eq" or a pedal key) under the given component,
// with the preset that matches the current settings ticked: the built-in ones, then the user's,
// with Save... to add the current settings and Delete to remove one.
void showModulePresetMenu (juce::AudioProcessorValueTreeState&, const juce::String& module, juce::Component& target);
