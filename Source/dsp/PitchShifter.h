#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"
#include "PeriodTracker.h"

// Delay-line pitch shifter: two read heads sweep through a 50 ms window at the pitch ratio,
// half a window apart, each faded by a sine-squared window so the heads hide their jumps.
// When a head jumps back it is offset by up to one period of the played note, so it lands
// in phase with the other head instead of cancelling it on low notes.
class PitchShifter
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled = rawParam (state, ParamIds::pitchOn);
        shift   = rawParam (state, ParamIds::pitchShift);
        detune  = rawParam (state, ParamIds::pitchDetune);
        mix     = rawParam (state, ParamIds::pitchMix);
    }

    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        tracker.prepare (monoSpec);
        window = (float) (monoSpec.sampleRate * windowSeconds);
        buffer.assign ((size_t) juce::nextPowerOfTwo ((int) (window + monoSpec.sampleRate / 25.0) + 4), 0.0f);
        mask = (int) buffer.size() - 1;
        writeIndex = 0;
        phase = firstOffset = secondOffset = 0.0f;

        active.reset (monoSpec.sampleRate, 0.03);
        active.setCurrentAndTargetValue (0.0f);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        active.setTargetValue (on ? 1.0f : 0.0f);

        if (! on && ! active.isSmoothing())
            return;

        const auto ratio = std::exp2 ((shift->load() + detune->load() / 100.0f) / 12.0f);
        const auto step = (1.0f - ratio) / window;
        const auto amount = mix->load();
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto input = data[i];
            buffer[(size_t) writeIndex] = input;
            tracker.push (input);

            const auto previous = phase;
            phase = wrap (phase + step);
            const auto other = wrap (phase + 0.5f);

            if (std::abs (phase - previous) > 0.5f)
                firstOffset = alignedOffset (phase, other, secondOffset);

            if (std::abs (other - wrap (previous + 0.5f)) > 0.5f)
                secondOffset = alignedOffset (other, phase, firstOffset);

            const auto first = std::sin (juce::MathConstants<float>::pi * phase);
            const auto second = std::sin (juce::MathConstants<float>::pi * other);
            const auto shifted = read (phase * window + 1.0f + firstOffset) * first * first
                               + read (other * window + 1.0f + secondOffset) * second * second;

            writeIndex = (writeIndex + 1) & mask;
            const auto wet = active.getNextValue() * amount;
            data[i] = input + wet * (shifted - input);
        }
    }

private:
    static constexpr double windowSeconds = 0.05;

    static float wrap (float value)
    {
        return value - std::floor (value);
    }

    // The offset, within one period, that puts a head that just jumped in phase with the other.
    float alignedOffset (float headPhase, float otherPhase, float otherOffset) const
    {
        const auto period = tracker.getPeriod();
        const auto difference = (otherPhase - headPhase) * window + otherOffset;
        return difference - std::floor (difference / period) * period;
    }

    float read (float delay) const
    {
        const auto position = (float) writeIndex - delay;
        const auto index = (int) std::floor (position);
        const auto fraction = position - (float) index;
        const auto a = buffer[(size_t) (index & mask)];
        const auto b = buffer[(size_t) ((index + 1) & mask)];
        return a + fraction * (b - a);
    }

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* shift = nullptr;
    std::atomic<float>* detune = nullptr;
    std::atomic<float>* mix = nullptr;

    PeriodTracker tracker;
    std::vector<float> buffer;
    int mask = 0, writeIndex = 0;
    float window = 1.0f, phase = 0.0f, firstOffset = 0.0f, secondOffset = 0.0f;
    juce::SmoothedValue<float> active;
};
