#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Asymmetric hard clip into a tanh stage, 4x oversampled, with a Muff-style
// low-pass/high-pass tone control and a clean blend to keep the low end.
class Fuzz
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::fuzzOn);
        amount  = rawParam (state, ParamIds::fuzzAmount);
        tone    = rawParam (state, ParamIds::fuzzTone);
        blend   = rawParam (state, ParamIds::fuzzBlend);
    }

    void prepare (double sampleRate, int maxBlockSize)
    {
        oversampling.initProcessing ((size_t) maxBlockSize);
        oversampling.reset();

        const auto factor = oversampling.getOversamplingFactor();
        const auto oversampledRate = sampleRate * (double) factor;
        const juce::dsp::ProcessSpec spec { oversampledRate, (juce::uint32) ((size_t) maxBlockSize * factor), 1 };

        preHighPass.setType (juce::dsp::StateVariableTPTFilterType::highpass);
        lowTone.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        highTone.setType (juce::dsp::StateVariableTPTFilterType::highpass);

        for (auto* filter : { &preHighPass, &lowTone, &highTone })
            filter->prepare (spec);

        preHighPass.setCutoffFrequency (90.0f);
        lowTone.setCutoffFrequency (500.0f);
        highTone.setCutoffFrequency (1100.0f);

        drive.reset (oversampledRate, 0.05);
        toneMix.reset (oversampledRate, 0.05);
        wet.reset (oversampledRate, 0.05);
        wet.setCurrentAndTargetValue (0.0f);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        drive.setTargetValue (juce::Decibels::decibelsToGain (20.0f + amount->load() * 40.0f));
        toneMix.setTargetValue (tone->load());
        wet.setTargetValue (on ? blend->load() : 0.0f);

        if (! on && ! wet.isSmoothing())
            return;

        auto upsampled = oversampling.processSamplesUp (monoBlock);
        auto* data = upsampled.getChannelPointer (0);

        for (size_t i = 0; i < upsampled.getNumSamples(); ++i)
        {
            const auto dry = data[i];
            auto shaped = juce::jlimit (-0.7f, 1.0f, preHighPass.processSample (0, dry) * drive.getNextValue());
            shaped = std::tanh (3.0f * shaped) * 0.8f;

            const auto t = toneMix.getNextValue();
            const auto fuzzed = (lowTone.processSample (0, shaped) * (1.0f - t) + highTone.processSample (0, shaped) * t) * wetLevel;

            const auto amountWet = wet.getNextValue();
            data[i] = dry * (1.0f - amountWet) + fuzzed * amountWet;
        }

        oversampling.processSamplesDown (monoBlock);
    }

private:
    static constexpr float wetLevel = 0.5f;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* amount = nullptr;
    std::atomic<float>* tone = nullptr;
    std::atomic<float>* blend = nullptr;

    juce::dsp::Oversampling<float> oversampling { 1, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true };
    juce::dsp::StateVariableTPTFilter<float> preHighPass, lowTone, highTone;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> drive { 10.0f };
    juce::SmoothedValue<float> toneMix, wet;
};
