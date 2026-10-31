#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Follows the period of a monophonic bass line, sample by sample: a comparator with
// hysteresis on the low-passed input, where each rising edge closes a period.
class PeriodTracker
{
public:
    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        filter.prepare (monoSpec);
        filter.reset();
        filter.setCutoffFrequency (300.0f);

        attackCoeff = timeCoefficient (0.002f, monoSpec.sampleRate);
        releaseCoeff = timeCoefficient (0.1f, monoSpec.sampleRate);
        minPeriod = (int) (monoSpec.sampleRate / 1000.0);
        maxPeriod = (int) (monoSpec.sampleRate / 25.0);

        period = (float) (monoSpec.sampleRate / 55.0);
        envelope = 0.0f;
        samplesSinceEdge = 0;
        high = false;
    }

    // Returns true on a rising edge, where a new cycle starts.
    bool push (float input)
    {
        const auto level = std::abs (input);
        envelope = level + (level > envelope ? attackCoeff : releaseCoeff) * (envelope - level);

        const auto tracked = filter.processSample (0, input);
        const auto hysteresis = envelope * 0.15f;
        ++samplesSinceEdge;

        if (! high && tracked > hysteresis)
        {
            high = true;

            if (samplesSinceEdge >= minPeriod && samplesSinceEdge <= maxPeriod)
                period = (float) samplesSinceEdge;

            samplesSinceEdge = 0;
            return true;
        }

        if (high && tracked < -hysteresis)
            high = false;

        return false;
    }

    float getPeriod() const     { return period; }
    float getEnvelope() const   { return envelope; }
    bool isHigh() const         { return high; }

private:
    juce::dsp::StateVariableTPTFilter<float> filter;
    float attackCoeff = 0.0f, releaseCoeff = 0.0f, envelope = 0.0f, period = 1.0f;
    int samplesSinceEdge = 0, minPeriod = 0, maxPeriod = 0;
    bool high = false;
};
