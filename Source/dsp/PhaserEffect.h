#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Four-stage phaser swept around the bass midrange, mixed 50/50 like the classic script pedals.
class PhaserEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled  = rawParam (state, ParamIds::phaserOn);
        rate     = rawParam (state, ParamIds::phaserRate);
        depth    = rawParam (state, ParamIds::phaserDepth);
        feedback = rawParam (state, ParamIds::phaserFeedback);
    }

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        phaser.prepare (spec);
        phaser.setCentreFrequency (700.0f);
        phaser.setMix (0.0f);
        phaser.reset();
    }

    void process (const juce::dsp::ProcessContextReplacing<float>& context)
    {
        phaser.setRate (rate->load());
        phaser.setDepth (depth->load());
        phaser.setFeedback (feedback->load());
        phaser.setMix (isOn (enabled) ? 0.5f : 0.0f);
        phaser.process (context);
    }

private:
    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* rate = nullptr;
    std::atomic<float>* depth = nullptr;
    std::atomic<float>* feedback = nullptr;

    juce::dsp::Phaser<float> phaser;
};
