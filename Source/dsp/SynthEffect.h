#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"
#include "PeriodTracker.h"

// Analog-style bass synth. The tracked period of the input clocks a square or a saw, scaled
// by the input envelope and run through a resonant 24 dB low-pass that the attack sweeps open.
class SynthEffect
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled   = rawParam (state, ParamIds::synthOn);
        wave      = rawParam (state, ParamIds::synthWave);
        cutoff    = rawParam (state, ParamIds::synthCutoff);
        resonance = rawParam (state, ParamIds::synthResonance);
        sweep     = rawParam (state, ParamIds::synthEnvelope);
        mix       = rawParam (state, ParamIds::synthMix);
    }

    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        sampleRate = monoSpec.sampleRate;
        tracker.prepare (monoSpec);

        for (auto* stage : { &first, &second })
        {
            stage->setType (juce::dsp::StateVariableTPTFilterType::lowpass);
            stage->prepare (monoSpec);
            stage->reset();
        }

        first.setResonance (0.7071f);
        active.reset (sampleRate, 0.03);
        active.setCurrentAndTargetValue (0.0f);
        phase = 0.0f;
        counter = controlInterval;
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        active.setTargetValue (on ? 1.0f : 0.0f);

        if (! on && ! active.isSmoothing())
            return;

        const auto saw = wave->load() > 0.5f;
        const auto baseCutoff = cutoff->load();
        const auto envelopeOctaves = sweep->load() * 4.0f;
        const auto amount = mix->load();
        const auto maxCutoff = (float) sampleRate * 0.45f;
        second.setResonance (0.7071f * std::pow (8.0f / 0.7071f, resonance->load()));
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto input = data[i];

            if (tracker.push (input))
                phase = 0.0f;

            phase += 1.0f / tracker.getPeriod();
            phase -= std::floor (phase);
            const auto envelope = tracker.getEnvelope();

            if (++counter >= controlInterval)
            {
                counter = 0;
                const auto opening = juce::jlimit (0.0f, 1.0f, envelope * 6.0f);
                const auto frequency = juce::jmin (maxCutoff, baseCutoff * std::exp2 (envelopeOctaves * opening));
                first.setCutoffFrequency (frequency);
                second.setCutoffFrequency (frequency);
            }

            const auto oscillator = saw ? 2.0f * phase - 1.0f : (tracker.isHigh() ? 1.0f : -1.0f);
            const auto voice = second.processSample (0, first.processSample (0, oscillator * envelope)) * outputGain;
            const auto wet = active.getNextValue() * amount;
            data[i] = input + wet * (voice - input);
        }
    }

private:
    static constexpr int controlInterval = 16;
    static constexpr float outputGain = 1.5f;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* wave = nullptr;
    std::atomic<float>* cutoff = nullptr;
    std::atomic<float>* resonance = nullptr;
    std::atomic<float>* sweep = nullptr;
    std::atomic<float>* mix = nullptr;

    double sampleRate = 44100.0;
    PeriodTracker tracker;
    juce::dsp::StateVariableTPTFilter<float> first, second;
    juce::SmoothedValue<float> active;
    float phase = 0.0f;
    int counter = 0;
};
