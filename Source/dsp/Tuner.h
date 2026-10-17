#pragma once

#include <juce_dsp/juce_dsp.h>

#include <optional>

// Pitch detection for the tuner. The audio thread only filters, decimates and queues
// samples; the YIN analysis runs on the message thread, so it adds no audio latency.
class Tuner
{
public:
    Tuner();

    void prepare (double sampleRate);
    void push (const float* samples, int numSamples);

    std::optional<float> detectFrequency();

private:
    static constexpr int fifoSize = 8192;
    static constexpr int windowSize = 1024;
    static constexpr int historySize = 2048;
    static constexpr float minFrequency = 28.0f;
    static constexpr float maxFrequency = 500.0f;

    juce::AbstractFifo fifo { fifoSize };
    std::vector<float> fifoBuffer;
    std::vector<float> history;
    std::vector<float> difference;

    juce::dsp::IIR::Filter<float> antiAliasFilter;
    int decimation = 1;
    int decimationPhase = 0;
    std::atomic<double> analysisRate { 11025.0 };
};
