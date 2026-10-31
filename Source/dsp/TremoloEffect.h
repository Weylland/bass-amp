#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Tremolo: an LFO on the output level, from a smooth sine to a choppy square.
class TremoloEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::tremoloOn);
        rate    = rawParam (state, ParamIds::tremoloRate);
        depth   = rawParam (state, ParamIds::tremoloDepth);
        shape   = rawParam (state, ParamIds::tremoloShape);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
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
        const auto increment = juce::MathConstants<float>::twoPi * rate->load() / (float) sampleRate;
        const auto drive = 1.0f + shape->load() * 12.0f;
        const auto normalise = 1.0f / std::tanh (drive);
        const auto amount = depth->load();

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            const auto lfo = std::tanh (drive * std::sin (phase)) * normalise;
            const auto gain = 1.0f - active.getNextValue() * amount * (0.5f - 0.5f * lfo);

            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
                block.getChannelPointer (channel)[i] *= gain;

            phase = std::fmod (phase + increment, juce::MathConstants<float>::twoPi);
        }
    }

private:
    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* rate = nullptr;
    std::atomic<float>* depth = nullptr;
    std::atomic<float>* shape = nullptr;

    double sampleRate = 44100.0;
    float phase = 0.0f;
    juce::SmoothedValue<float> active;
};
