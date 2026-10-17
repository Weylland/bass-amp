#pragma once

#include <juce_dsp/juce_dsp.h>

// Asymmetric tanh clipper, 2x oversampled, with a dry/wet blend to keep the low end intact.
class Distortion
{
public:
    void prepare (double sampleRate, int maxBlockSize)
    {
        oversampling.initProcessing ((size_t) maxBlockSize);

        const auto factor = oversampling.getOversamplingFactor();
        const auto oversampledRate = sampleRate * (double) factor;

        toneFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        toneFilter.prepare ({ oversampledRate, (juce::uint32) ((size_t) maxBlockSize * factor), 1 });

        driveGain.reset (oversampledRate, 0.05);
        blend.reset (oversampledRate, 0.05);
        blend.setCurrentAndTargetValue (0.0f);

        oversampling.reset();
    }

    void setParameters (bool enabled, float drive, float toneHz, float blendAmount)
    {
        driveGain.setTargetValue (juce::Decibels::decibelsToGain (drive * 40.0f));
        blend.setTargetValue (enabled ? blendAmount : 0.0f);
        toneFilter.setCutoffFrequency (toneHz);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        auto upsampled = oversampling.processSamplesUp (monoBlock);
        auto* data = upsampled.getChannelPointer (0);
        const auto biasOffset = std::tanh (bias);

        for (size_t i = 0; i < upsampled.getNumSamples(); ++i)
        {
            const auto dry = data[i];
            auto wet = std::tanh (driveGain.getNextValue() * dry + bias) - biasOffset;
            wet = toneFilter.processSample (0, wet) * wetLevel;

            const auto amount = blend.getNextValue();
            data[i] = dry * (1.0f - amount) + wet * amount;
        }

        oversampling.processSamplesDown (monoBlock);
    }

private:
    static constexpr float bias = 0.2f;
    static constexpr float wetLevel = 0.6f;

    juce::dsp::Oversampling<float> oversampling { 1, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true };
    juce::dsp::StateVariableTPTFilter<float> toneFilter;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> driveGain { 1.0f };
    juce::SmoothedValue<float> blend;
};
