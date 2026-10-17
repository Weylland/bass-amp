#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "ParameterIds.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

inline std::atomic<float>* rawParam (juce::AudioProcessorValueTreeState& state, const char* paramId)
{
    auto* value = state.getRawParameterValue (paramId);
    jassert (value != nullptr);
    return value;
}

inline bool isOn (const std::atomic<float>* toggle)
{
    return toggle->load() > 0.5f;
}

inline float timeCoefficient (float seconds, double sampleRate)
{
    return std::exp (-1.0f / (seconds * (float) sampleRate));
}
