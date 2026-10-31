#pragma once

#include <juce_dsp/juce_dsp.h>

// Click track for practice: a short sine blip on every beat, higher on the first beat of the
// bar. It is mixed in after the recorder so it never ends up in recordings.
class Metronome
{
public:
    std::atomic<bool> enabled { false };
    std::atomic<float> bpm { 100.0f };
    std::atomic<float> volume { 0.5f };
    std::atomic<int> beatsPerBar { 4 };

    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        decay = std::exp (-1.0f / (0.012f * (float) sampleRate));
        reset();
    }

    // The beat last clicked, from 0, or -1 while stopped.
    int getBeat() const { return currentBeat.load(); }

    // Audio thread, before process() in the same block: samples until the next bar starts
    // (0 when it starts on the first sample), and the length of a bar.
    double getSamplesToNextBar() const
    {
        return samplesToNextBeat + (beat < 0 ? 0.0 : (double) (juce::jmax (1, beatsPerBar.load()) - 1 - beat) * getBeatLength());
    }

    double getBarLength() const { return getBeatLength() * juce::jmax (1, beatsPerBar.load()); }

    void process (juce::dsp::AudioBlock<float>& block)
    {
        if (! enabled.load())
        {
            reset();
            return;
        }

        const auto beatLength = getBeatLength();
        const auto gain = volume.load();

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            if (samplesToNextBeat <= 0.0)
            {
                samplesToNextBeat += beatLength;
                beat = (beat + 1) % juce::jmax (1, beatsPerBar.load());
                currentBeat.store (beat);
                envelope = 1.0f;
                clickPhase = 0.0f;
                clickIncrement = juce::MathConstants<float>::twoPi * (beat == 0 ? 2000.0f : 1400.0f) / (float) sampleRate;
            }

            samplesToNextBeat -= 1.0;

            if (envelope < 0.001f)
                continue;

            const auto click = std::sin (clickPhase) * envelope * gain;
            clickPhase += clickIncrement;
            envelope *= decay;

            for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
                block.getChannelPointer (channel)[i] += click;
        }
    }

private:
    double getBeatLength() const { return sampleRate * 60.0 / (double) juce::jlimit (30.0f, 300.0f, bpm.load()); }

    void reset()
    {
        samplesToNextBeat = 0.0;
        beat = -1;
        envelope = 0.0f;
        currentBeat.store (-1);
    }

    double sampleRate = 44100.0, samplesToNextBeat = 0.0;
    int beat = -1;
    float envelope = 0.0f, decay = 0.0f, clickPhase = 0.0f, clickIncrement = 0.0f;
    std::atomic<int> currentBeat { -1 };
};
