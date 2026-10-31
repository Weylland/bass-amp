#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Safety limiter on the output: instant attack, 80 ms release, linked across channels,
// ceiling at -1 dBFS. Transparent below the ceiling, and it adds no latency.
class OutputLimiter
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::limiterOn);
    }

    void prepare (double sampleRate)
    {
        releaseCoeff = timeCoefficient (0.08f, sampleRate);
        gain = 1.0f;
    }

    void process (juce::dsp::AudioBlock<float>& block)
    {
        if (! isOn (enabled))
        {
            gain = 1.0f;
            return;
        }

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            auto peak = 0.0f;

            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
                peak = juce::jmax (peak, std::abs (block.getChannelPointer (channel)[i]));

            const auto target = peak > ceiling ? ceiling / peak : 1.0f;
            gain = target < gain ? target : target + releaseCoeff * (gain - target);

            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
                block.getChannelPointer (channel)[i] *= gain;
        }
    }

private:
    static constexpr float ceiling = 0.891f;   // -1 dBFS

    std::atomic<float>* enabled = nullptr;
    float releaseCoeff = 0.0f, gain = 1.0f;
};
