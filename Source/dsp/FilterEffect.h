#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Filter pedal: low-, band- or high-pass at 12 or 24 dB/oct (two cascaded state-variable
// stages, the resonance on the second), with an LFO sweeping the cutoff up to two octaves
// either way.
class FilterEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled   = rawParam (state, ParamIds::filterOn);
        type      = rawParam (state, ParamIds::filterType);
        cutoff    = rawParam (state, ParamIds::filterCutoff);
        resonance = rawParam (state, ParamIds::filterResonance);
        rate      = rawParam (state, ParamIds::filterRate);
        depth     = rawParam (state, ParamIds::filterDepth);
    }

    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        sampleRate = monoSpec.sampleRate;

        for (auto* stage : { &first, &second })
        {
            stage->prepare (monoSpec);
            stage->reset();
        }

        first.setResonance (butterworthQ);
        wet.reset (sampleRate, 0.03);
        wet.setCurrentAndTargetValue (0.0f);
        lfoPhase = 0.0f;
        counter = controlInterval;
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        wet.setTargetValue (on ? 1.0f : 0.0f);

        if (! on && ! wet.isSmoothing())
            return;

        using Type = juce::dsp::StateVariableTPTFilterType;
        const auto index = juce::jlimit (0, 5, juce::roundToInt (type->load()));
        const auto filterType = std::array { Type::lowpass, Type::bandpass, Type::highpass }[(size_t) index / 2];
        const auto steep = index % 2 == 1;

        first.setType (filterType);
        second.setType (filterType);
        second.setResonance (butterworthQ * std::pow (maxQ / butterworthQ, resonance->load()));

        const auto baseCutoff = cutoff->load();
        const auto sweep = depth->load() * 2.0f;
        const auto lfoIncrement = juce::MathConstants<float>::twoPi * rate->load() * (float) controlInterval / (float) sampleRate;
        const auto maxCutoff = (float) sampleRate * 0.45f;
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            if (++counter >= controlInterval)
            {
                counter = 0;
                lfoPhase = std::fmod (lfoPhase + lfoIncrement, juce::MathConstants<float>::twoPi);
                const auto frequency = juce::jlimit (20.0f, maxCutoff, baseCutoff * std::exp2 (sweep * std::sin (lfoPhase)));
                first.setCutoffFrequency (frequency);
                second.setCutoffFrequency (frequency);
            }

            const auto input = data[i];
            const auto filtered = second.processSample (0, steep ? first.processSample (0, input) : input);
            data[i] = input + wet.getNextValue() * (filtered - input);
        }
    }

private:
    static constexpr float butterworthQ = 0.7071f;
    static constexpr float maxQ = 8.0f;
    static constexpr int controlInterval = 16;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* type = nullptr;
    std::atomic<float>* cutoff = nullptr;
    std::atomic<float>* resonance = nullptr;
    std::atomic<float>* rate = nullptr;
    std::atomic<float>* depth = nullptr;

    double sampleRate = 44100.0;
    juce::dsp::StateVariableTPTFilter<float> first, second;
    juce::SmoothedValue<float> wet;
    float lfoPhase = 0.0f;
    int counter = 0;
};
