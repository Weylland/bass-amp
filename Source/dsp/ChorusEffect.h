#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

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
    }

    void process (const juce::dsp::ProcessContextReplacing<float>& context)
    {
        chorus.setRate (rate->load());
        chorus.setDepth (depth->load());
        chorus.setMix (isOn (enabled) ? mix->load() : 0.0f);
        chorus.process (context);
    }

private:
    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* rate = nullptr;
    std::atomic<float>* depth = nullptr;
    std::atomic<float>* mix = nullptr;

    juce::dsp::Chorus<float> chorus;
};
