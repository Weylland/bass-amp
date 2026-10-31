#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Stereo flanger: a 0.3 to 5 ms delay swept by an LFO (the right channel a quarter cycle
// ahead), with feedback for the jet sweep.
class FlangerEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled  = rawParam (state, ParamIds::flangerOn);
        rate     = rawParam (state, ParamIds::flangerRate);
        depth    = rawParam (state, ParamIds::flangerDepth);
        feedback = rawParam (state, ParamIds::flangerFeedback);
        mix      = rawParam (state, ParamIds::flangerMix);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        const auto size = juce::nextPowerOfTwo ((int) (sampleRate * maxDelaySeconds) + 4);
        lines.assign (spec.numChannels, std::vector<float> ((size_t) size, 0.0f));
        mask = size - 1;
        writeIndex = 0;
        phase = 0.0f;

        active.reset (sampleRate, 0.03);
        active.setCurrentAndTargetValue (0.0f);
    }

    void process (const juce::dsp::ProcessContextReplacing<float>& context)
    {
        const auto on = isOn (enabled);
        active.setTargetValue (on ? 1.0f : 0.0f);

        if (! on && ! active.isSmoothing())
            return;

        auto& block = context.getOutputBlock();
        const auto numChannels = juce::jmin (block.getNumChannels(), lines.size());
        const auto increment = juce::MathConstants<float>::twoPi * rate->load() / (float) sampleRate;
        const auto sweepMs = depth->load() * 4.7f;
        const auto regen = feedback->load();
        const auto amount = mix->load();

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            const auto wet = active.getNextValue() * amount;

            for (size_t channel = 0; channel < numChannels; ++channel)
            {
                auto& line = lines[channel];
                const auto lfo = std::sin (phase + (float) channel * juce::MathConstants<float>::halfPi);
                const auto delay = (0.3f + sweepMs * (0.5f + 0.5f * lfo)) * 0.001f * (float) sampleRate;

                const auto position = (float) writeIndex - delay;
                const auto index = (int) std::floor (position);
                const auto fraction = position - (float) index;
                const auto a = line[(size_t) (index & mask)];
                const auto delayed = a + fraction * (line[(size_t) ((index + 1) & mask)] - a);

                auto* data = block.getChannelPointer (channel);
                line[(size_t) writeIndex] = data[i] + delayed * regen;
                data[i] = data[i] * (1.0f - 0.5f * wet) + delayed * 0.5f * wet;
            }

            writeIndex = (writeIndex + 1) & mask;
            phase = std::fmod (phase + increment, juce::MathConstants<float>::twoPi);
        }
    }

private:
    static constexpr double maxDelaySeconds = 0.01;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* rate = nullptr;
    std::atomic<float>* depth = nullptr;
    std::atomic<float>* feedback = nullptr;
    std::atomic<float>* mix = nullptr;

    double sampleRate = 44100.0;
    std::vector<std::vector<float>> lines;
    int mask = 0, writeIndex = 0;
    float phase = 0.0f;
    juce::SmoothedValue<float> active;
};
