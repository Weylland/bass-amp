#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Bass wah: a resonant band-pass swept from 200 Hz (heel) to 2.2 kHz (toe), with a clean
// blend under it to keep the low end. The pedal position is smoothed so an expression
// pedal sweeps without zipper noise.
class WahEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled   = rawParam (state, ParamIds::wahOn);
        pedal     = rawParam (state, ParamIds::wahPedal);
        resonance = rawParam (state, ParamIds::wahResonance);
        blend     = rawParam (state, ParamIds::wahBlend);
    }

    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        filter.setType (juce::dsp::StateVariableTPTFilterType::bandpass);
        filter.prepare (monoSpec);
        filter.reset();

        position.reset (monoSpec.sampleRate, 0.03);
        position.setCurrentAndTargetValue (pedal->load());
        active.reset (monoSpec.sampleRate, 0.03);
        active.setCurrentAndTargetValue (0.0f);
        counter = controlInterval;
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        active.setTargetValue (on ? 1.0f : 0.0f);

        if (! on && ! active.isSmoothing())
            return;

        position.setTargetValue (pedal->load());
        filter.setResonance (resonance->load());
        const auto clean = blend->load();
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto travel = position.getNextValue();

            if (++counter >= controlInterval)
            {
                counter = 0;
                filter.setCutoffFrequency (heelHz * std::pow (toeHz / heelHz, travel));
            }

            const auto input = data[i];
            const auto wah = input * clean + filter.processSample (0, input) * peakGain * (1.0f - clean);
            data[i] = input + active.getNextValue() * (wah - input);
        }
    }

private:
    static constexpr float heelHz = 200.0f;
    static constexpr float toeHz = 2200.0f;
    static constexpr float peakGain = 3.0f;
    static constexpr int controlInterval = 16;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* pedal = nullptr;
    std::atomic<float>* resonance = nullptr;
    std::atomic<float>* blend = nullptr;

    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::SmoothedValue<float> position, active;
    int counter = 0;
};
