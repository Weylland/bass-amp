#pragma once

#include <atomic>
#include <cmath>

// Lock-free peak holder: the audio thread pushes, the UI takes the peak since its last read.
class PeakMeter
{
public:
    void push (const float* samples, int numSamples)
    {
        auto peak = 0.0f;

        for (int i = 0; i < numSamples; ++i)
            peak = std::fmax (peak, std::abs (samples[i]));

        auto previous = value.load();

        while (peak > previous && ! value.compare_exchange_weak (previous, peak)) {}
    }

    float takePeak() { return value.exchange (0.0f); }

private:
    std::atomic<float> value { 0.0f };
};
