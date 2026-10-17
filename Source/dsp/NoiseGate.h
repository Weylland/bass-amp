#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Downward gate with hysteresis: opens in 1 ms, closes over the release time.
class NoiseGate
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled   = rawParam (state, ParamIds::gateOn);
        threshold = rawParam (state, ParamIds::gateThreshold);
        release   = rawParam (state, ParamIds::gateRelease);
    }

    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        attackCoeff = timeCoefficient (0.001f, sampleRate);
        envelopeCoeff = timeCoefficient (0.02f, sampleRate);
        envelope = 0.0f;
        gain = 1.0f;
        isOpen = true;
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto active = isOn (enabled);

        if (! active && gain >= 1.0f)
            return;

        const auto openLevel = juce::Decibels::decibelsToGain (threshold->load());
        const auto closeLevel = openLevel * 0.6f;
        const auto releaseCoeff = timeCoefficient (release->load() * 0.001f, sampleRate);
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto level = std::abs (data[i]);
            envelope = level > envelope ? level : level + envelopeCoeff * (envelope - level);

            if (envelope > openLevel)
                isOpen = true;
            else if (envelope < closeLevel)
                isOpen = false;

            const auto target = (! active || isOpen) ? 1.0f : 0.0f;
            const auto coeff = target > gain ? attackCoeff : releaseCoeff;
            gain = juce::jmin (1.0f, target + coeff * (gain - target));

            if (target >= 1.0f && gain > 0.9999f)
                gain = 1.0f;

            data[i] *= gain;
        }
    }

private:
    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* threshold = nullptr;
    std::atomic<float>* release = nullptr;

    double sampleRate = 44100.0;
    float attackCoeff = 0.0f, envelopeCoeff = 0.0f;
    float envelope = 0.0f, gain = 1.0f;
    bool isOpen = true;
};
