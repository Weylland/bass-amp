#pragma once

#include <juce_core/juce_core.h>

#include <vector>

// Lock-free queue of output samples for the spectrum display: the audio thread pushes,
// the UI pulls what is there. When the UI is not reading, new samples are dropped.
class SpectrumTap
{
public:
    void prepare (double newSampleRate)
    {
        sampleRate.store (newSampleRate);
    }

    double getSampleRate() const { return sampleRate.load(); }

    void push (const float* samples, int numSamples)
    {
        const auto scope = fifo.write (juce::jmin (numSamples, fifo.getFreeSpace()));

        if (scope.blockSize1 > 0)
            std::copy_n (samples, scope.blockSize1, buffer.begin() + scope.startIndex1);

        if (scope.blockSize2 > 0)
            std::copy_n (samples + scope.blockSize1, scope.blockSize2, buffer.begin() + scope.startIndex2);
    }

    int pull (float* destination, int maxSamples)
    {
        const auto scope = fifo.read (juce::jmin (maxSamples, fifo.getNumReady()));

        if (scope.blockSize1 > 0)
            std::copy_n (buffer.begin() + scope.startIndex1, scope.blockSize1, destination);

        if (scope.blockSize2 > 0)
            std::copy_n (buffer.begin() + scope.startIndex2, scope.blockSize2, destination + scope.blockSize1);

        return scope.blockSize1 + scope.blockSize2;
    }

private:
    static constexpr int capacity = 16384;

    std::atomic<double> sampleRate { 48000.0 };
    juce::AbstractFifo fifo { capacity };
    std::vector<float> buffer = std::vector<float> ((size_t) capacity, 0.0f);
};
