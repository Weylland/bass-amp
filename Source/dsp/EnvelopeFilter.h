#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Auto-wah: a resonant low-pass whose cutoff follows the playing dynamics.
class EnvelopeFilter
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled     = rawParam (state, ParamIds::envOn);
        sensitivity = rawParam (state, ParamIds::envSensitivity);
        resonance   = rawParam (state, ParamIds::envResonance);
        mix         = rawParam (state, ParamIds::envMix);
    }

    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        filter.prepare (monoSpec);
        filter.setCutoffFrequency (minCutoff);

        attackCoeff = timeCoefficient (0.003f, monoSpec.sampleRate);
        releaseCoeff = timeCoefficient (0.12f, monoSpec.sampleRate);

        wet.reset (monoSpec.sampleRate, 0.05);
        wet.setCurrentAndTargetValue (0.0f);
        envelope = 0.0f;
        counter = 0;
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        wet.setTargetValue (on ? mix->load() : 0.0f);

        if (! on && ! wet.isSmoothing())
            return;

        const auto sensitivityGain = juce::Decibels::decibelsToGain (sensitivity->load() * 36.0f);
        filter.setResonance (resonance->load());
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto input = data[i];
            const auto level = std::abs (input);
            envelope = level + (level > envelope ? attackCoeff : releaseCoeff) * (envelope - level);

            if (++counter >= controlInterval)
            {
                counter = 0;
                const auto amount = juce::jlimit (0.0f, 1.0f, envelope * sensitivityGain);
                filter.setCutoffFrequency (minCutoff * std::pow (maxCutoff / minCutoff, amount));
            }

            const auto amount = wet.getNextValue();
            data[i] = input * (1.0f - amount) + filter.processSample (0, input) * amount;
        }
    }

private:
    static constexpr float minCutoff = 120.0f;
    static constexpr float maxCutoff = 3500.0f;
    static constexpr int controlInterval = 16;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* sensitivity = nullptr;
    std::atomic<float>* resonance = nullptr;
    std::atomic<float>* mix = nullptr;

    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::SmoothedValue<float> wet;
    float attackCoeff = 0.0f, releaseCoeff = 0.0f, envelope = 0.0f;
    int counter = 0;
};
