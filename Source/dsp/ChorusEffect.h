#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Bass chorus: a Linkwitz-Riley crossover keeps the lows dry and centred, only the
// content above it is chorused, so the low end stays tight.
class ChorusEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::chorusOn);
        rate    = rawParam (state, ParamIds::chorusRate);
        depth   = rawParam (state, ParamIds::chorusDepth);
        mix     = rawParam (state, ParamIds::chorusMix);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        chorus.prepare (spec);
        chorus.setCentreDelay (7.0f);
        chorus.setFeedback (0.0f);
        chorus.setMix (0.0f);
        chorus.reset();

        crossover.setType (juce::dsp::LinkwitzRileyFilterType::lowpass);
        crossover.setCutoffFrequency (crossoverHz);
        crossover.prepare (spec);

        highBand.setSize ((int) spec.numChannels, (int) spec.maximumBlockSize);
    }

    void process (const juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numChannels = juce::jmin (block.getNumChannels(), (size_t) highBand.getNumChannels());
        const auto numSamples = block.getNumSamples();

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* data = block.getChannelPointer (channel);
            auto* high = highBand.getWritePointer ((int) channel);

            for (size_t i = 0; i < numSamples; ++i)
                crossover.processSample ((int) channel, data[i], data[i], high[i]);
        }

        auto highBlock = juce::dsp::AudioBlock<float> (highBand).getSubBlock (0, numSamples).getSubsetChannelBlock (0, numChannels);
        chorus.setRate (rate->load());
        chorus.setDepth (depth->load());
        chorus.setMix (isOn (enabled) ? mix->load() : 0.0f);
        chorus.process (juce::dsp::ProcessContextReplacing<float> (highBlock));

        block.getSubsetChannelBlock (0, numChannels).add (highBlock);
    }

private:
    static constexpr float crossoverHz = 200.0f;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* rate = nullptr;
    std::atomic<float>* depth = nullptr;
    std::atomic<float>* mix = nullptr;

    juce::dsp::Chorus<float> chorus;
    juce::dsp::LinkwitzRileyFilter<float> crossover;
    juce::AudioBuffer<float> highBand;
};
