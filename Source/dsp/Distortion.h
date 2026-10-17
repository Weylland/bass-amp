#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Asymmetric tanh clipper, 2x oversampled, with a dry/wet blend to keep the low end intact.
class Distortion
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::distOn);
        drive   = rawParam (state, ParamIds::distDrive);
        tone    = rawParam (state, ParamIds::distTone);
        blend   = rawParam (state, ParamIds::distBlend);
    }

    void prepare (double sampleRate, int maxBlockSize)
    {
        oversampling.initProcessing ((size_t) maxBlockSize);
        oversampling.reset();

        const auto factor = oversampling.getOversamplingFactor();
        const auto oversampledRate = sampleRate * (double) factor;

        toneFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        toneFilter.prepare ({ oversampledRate, (juce::uint32) ((size_t) maxBlockSize * factor), 1 });

        driveGain.reset (oversampledRate, 0.05);
        wet.reset (oversampledRate, 0.05);
        wet.setCurrentAndTargetValue (0.0f);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        driveGain.setTargetValue (juce::Decibels::decibelsToGain (drive->load() * 40.0f));
        wet.setTargetValue (on ? blend->load() : 0.0f);
        toneFilter.setCutoffFrequency (tone->load());

        if (! on && ! wet.isSmoothing())
            return;

        auto upsampled = oversampling.processSamplesUp (monoBlock);
        auto* data = upsampled.getChannelPointer (0);
        const auto biasOffset = std::tanh (bias);

        for (size_t i = 0; i < upsampled.getNumSamples(); ++i)
        {
            const auto dry = data[i];
            auto distorted = std::tanh (driveGain.getNextValue() * dry + bias) - biasOffset;
            distorted = toneFilter.processSample (0, distorted) * wetLevel;

            const auto amount = wet.getNextValue();
            data[i] = dry * (1.0f - amount) + distorted * amount;
        }

        oversampling.processSamplesDown (monoBlock);
    }

private:
    static constexpr float bias = 0.2f;
    static constexpr float wetLevel = 0.6f;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* drive = nullptr;
    std::atomic<float>* tone = nullptr;
    std::atomic<float>* blend = nullptr;

    juce::dsp::Oversampling<float> oversampling { 1, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true };
    juce::dsp::StateVariableTPTFilter<float> toneFilter;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> driveGain { 1.0f };
    juce::SmoothedValue<float> wet;
};
