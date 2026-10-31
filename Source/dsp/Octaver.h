#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Analog-style octaver. Sub: a flip-flop toggled on each cycle of the low-passed input
// gives a square wave at half the frequency, shaped by the input envelope.
// Up: full-wave rectification doubles the frequency, like the classic octave-up fuzz.
class Octaver
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::octOn);
        sub     = rawParam (state, ParamIds::octSub);
        dry     = rawParam (state, ParamIds::octDry);
        up      = rawParam (state, ParamIds::octUp);
    }

    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        for (auto* filter : { &trackingFilter, &subFilter })
        {
            filter->setType (juce::dsp::StateVariableTPTFilterType::lowpass);
            filter->prepare (monoSpec);
        }

        trackingFilter.setCutoffFrequency (250.0f);
        subFilter.setCutoffFrequency (220.0f);

        *upDcBlocker.coefficients = juce::dsp::IIR::ArrayCoefficients<float>::makeFirstOrderHighPass (monoSpec.sampleRate, 30.0f);
        upDcBlocker.prepare (monoSpec);

        attackCoeff = timeCoefficient (0.005f, monoSpec.sampleRate);
        releaseCoeff = timeCoefficient (0.06f, monoSpec.sampleRate);

        subLevel.reset (monoSpec.sampleRate, 0.05);
        upLevel.reset (monoSpec.sampleRate, 0.05);
        dryLevel.reset (monoSpec.sampleRate, 0.05);
        subLevel.setCurrentAndTargetValue (0.0f);
        upLevel.setCurrentAndTargetValue (0.0f);
        dryLevel.setCurrentAndTargetValue (1.0f);

        envelope = 0.0f;
        flipFlop = 1.0f;
        isHigh = false;
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        subLevel.setTargetValue (on ? sub->load() : 0.0f);
        upLevel.setTargetValue (on ? up->load() : 0.0f);
        dryLevel.setTargetValue (on ? dry->load() : 1.0f);

        if (! on && ! subLevel.isSmoothing() && ! upLevel.isSmoothing() && ! dryLevel.isSmoothing())
            return;

        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto input = data[i];
            const auto level = std::abs (input);
            envelope = level + (level > envelope ? attackCoeff : releaseCoeff) * (envelope - level);

            const auto tracked = trackingFilter.processSample (0, input);
            const auto hysteresis = envelope * 0.15f;

            if (! isHigh && tracked > hysteresis)
            {
                isHigh = true;
                flipFlop = -flipFlop;
            }
            else if (isHigh && tracked < -hysteresis)
            {
                isHigh = false;
            }

            const auto octave = subFilter.processSample (0, flipFlop * envelope);
            const auto octaveUp = upDcBlocker.processSample (std::abs (input));
            data[i] = input * dryLevel.getNextValue() + octave * subLevel.getNextValue() * 1.5f
                    + octaveUp * upLevel.getNextValue() * 2.0f;
        }
    }

private:
    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* sub = nullptr;
    std::atomic<float>* dry = nullptr;
    std::atomic<float>* up = nullptr;

    juce::dsp::StateVariableTPTFilter<float> trackingFilter, subFilter;
    juce::dsp::IIR::Filter<float> upDcBlocker;
    juce::SmoothedValue<float> subLevel, upLevel, dryLevel;
    float attackCoeff = 0.0f, releaseCoeff = 0.0f;
    float envelope = 0.0f, flipFlop = 1.0f;
    bool isHigh = false;
};
