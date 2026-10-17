#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

class ReverbEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::reverbOn);
        size    = rawParam (state, ParamIds::reverbSize);
        damping = rawParam (state, ParamIds::reverbDamping);
        mix     = rawParam (state, ParamIds::reverbMix);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        reverb.prepare (spec);
        reverb.reset();
    }

    void process (const juce::dsp::ProcessContextReplacing<float>& context)
    {
        // juce::Reverb scales dry by 2 and wet by 3: dryLevel 0.5 is unity.
        const auto amount = isOn (enabled) ? mix->load() : 0.0f;

        juce::dsp::Reverb::Parameters parameters;
        parameters.roomSize = size->load();
        parameters.damping = damping->load();
        parameters.wetLevel = amount * 0.66f;
        parameters.dryLevel = 0.5f * (1.0f - 0.4f * amount);
        parameters.width = 1.0f;

        reverb.setParameters (parameters);
        reverb.process (context);
    }

private:
    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* size = nullptr;
    std::atomic<float>* damping = nullptr;
    std::atomic<float>* mix = nullptr;

    juce::dsp::Reverb reverb;
};
