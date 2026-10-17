#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Stereo feedback delay with damped repeats. Switching it off stops feeding new
// signal in but lets the existing repeats fade out.
class Delay
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled  = rawParam (state, ParamIds::delayOn);
        time     = rawParam (state, ParamIds::delayTime);
        feedback = rawParam (state, ParamIds::delayFeedback);
        mix      = rawParam (state, ParamIds::delayMix);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        delayLine.setMaximumDelayInSamples ((int) std::ceil (sampleRate * maxDelaySeconds) + 1);
        delayLine.prepare (spec);
        delayLine.reset();

        damping.assign (spec.numChannels, 0.0f);
        delaySamples.reset (sampleRate, 0.15);
        delaySamples.setCurrentAndTargetValue (time->load() * 0.001f * (float) sampleRate);
        input.reset (sampleRate, 0.02);
        input.setCurrentAndTargetValue (0.0f);
    }

    void process (const juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numSamples = (int) block.getNumSamples();
        const auto numChannels = juce::jmin ((int) block.getNumChannels(), (int) damping.size());

        input.setTargetValue (isOn (enabled) ? 1.0f : 0.0f);
        delaySamples.setTargetValue (time->load() * 0.001f * (float) sampleRate);

        const auto feedbackGain = feedback->load();
        const auto wet = mix->load();

        for (int i = 0; i < numSamples; ++i)
        {
            delayLine.setDelay (delaySamples.getNextValue());
            const auto send = input.getNextValue();

            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* data = block.getChannelPointer ((size_t) channel);
                const auto delayed = delayLine.popSample (channel);
                damping[(size_t) channel] += dampingCoeff * (delayed - damping[(size_t) channel]);

                const auto repeat = damping[(size_t) channel];
                delayLine.pushSample (channel, data[i] * send + repeat * feedbackGain);
                data[i] += repeat * wet;
            }
        }
    }

private:
    static constexpr double maxDelaySeconds = 1.5;
    static constexpr float dampingCoeff = 0.45f;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* time = nullptr;
    std::atomic<float>* feedback = nullptr;
    std::atomic<float>* mix = nullptr;

    double sampleRate = 44100.0;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    std::vector<float> damping;
    juce::SmoothedValue<float> delaySamples, input;
};
