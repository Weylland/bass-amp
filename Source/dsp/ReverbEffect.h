#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Room reverb fed through a low cut, so the tail adds space without washing out the low end.
class ReverbEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::reverbOn);
        size    = rawParam (state, ParamIds::reverbSize);
        damping = rawParam (state, ParamIds::reverbDamping);
        mix     = rawParam (state, ParamIds::reverbMix);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        reverb.prepare (spec);
        reverb.reset();

        lowCut.setType (juce::dsp::StateVariableTPTFilterType::highpass);
        lowCut.setCutoffFrequency (lowCutHz);
        lowCut.prepare (spec);

        wetBuffer.setSize ((int) spec.numChannels, (int) spec.maximumBlockSize);
        dryGain.reset (spec.sampleRate, 0.05);
        dryGain.setCurrentAndTargetValue (1.0f);
    }

    void process (const juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numChannels = juce::jmin (block.getNumChannels(), (size_t) wetBuffer.getNumChannels());
        const auto amount = isOn (enabled) ? mix->load() : 0.0f;

        auto wet = juce::dsp::AudioBlock<float> (wetBuffer).getSubBlock (0, block.getNumSamples()).getSubsetChannelBlock (0, numChannels);
        wet.copyFrom (block.getSubsetChannelBlock (0, numChannels));
        lowCut.process (juce::dsp::ProcessContextReplacing<float> (wet));

        // juce::Reverb scales wet by 3: the wet level below gives about 2x the mix amount.
        juce::dsp::Reverb::Parameters parameters;
        parameters.roomSize = size->load();
        parameters.damping = damping->load();
        parameters.wetLevel = amount * 0.66f;
        parameters.dryLevel = 0.0f;
        parameters.width = 1.0f;
        reverb.setParameters (parameters);
        reverb.process (juce::dsp::ProcessContextReplacing<float> (wet));

        dryGain.setTargetValue (1.0f - 0.4f * amount);
        block.multiplyBy (dryGain);
        block.getSubsetChannelBlock (0, numChannels).add (wet);
    }

private:
    static constexpr float lowCutHz = 180.0f;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* size = nullptr;
    std::atomic<float>* damping = nullptr;
    std::atomic<float>* mix = nullptr;

    juce::dsp::Reverb reverb;
    juce::dsp::StateVariableTPTFilter<float> lowCut;
    juce::AudioBuffer<float> wetBuffer;
    juce::SmoothedValue<float> dryGain;
};
