#include "Tuner.h"

Tuner::Tuner()
    : fifoBuffer ((size_t) fifoSize, 0.0f),
      history ((size_t) historySize, 0.0f)
{
}

void Tuner::prepare (double sampleRate)
{
    decimation = juce::jmax (1, juce::roundToInt (sampleRate / 11025.0));
    decimationPhase = 0;
    analysisRate.store (sampleRate / decimation);

    *antiAliasFilter.coefficients = juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass (sampleRate, 900.0f, 0.707f);
    antiAliasFilter.reset();
    fifo.reset();
}

void Tuner::push (const float* samples, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        const auto filtered = antiAliasFilter.processSample (samples[i]);

        if (++decimationPhase < decimation)
            continue;

        decimationPhase = 0;
        const auto scope = fifo.write (1);

        if (scope.blockSize1 > 0)
            fifoBuffer[(size_t) scope.startIndex1] = filtered;
    }
}

std::optional<float> Tuner::detectFrequency()
{
    const auto numReady = fifo.getNumReady();

    if (numReady > 0)
    {
        const auto numNew = juce::jmin (numReady, historySize);
        std::move (history.begin() + numNew, history.end(), history.begin());

        auto writeIndex = (size_t) (historySize - numNew);
        const auto scope = fifo.read (numReady);
        const auto skip = numReady - numNew;
        auto readCount = 0;

        scope.forEach ([&] (int index)
        {
            if (readCount++ >= skip)
                history[writeIndex++] = fifoBuffer[(size_t) index];
        });
    }

    const auto rate = (float) analysisRate.load();
    const auto maxTau = juce::jmin ((int) (rate / minFrequency), historySize - windowSize - 1);
    const auto minTau = juce::jmax (2, (int) (rate / maxFrequency));
    const auto* x = history.data() + (historySize - windowSize - maxTau);

    auto energy = 0.0f;

    for (int j = 0; j < windowSize + maxTau; ++j)
        energy += x[j] * x[j];

    if (std::sqrt (energy / (float) (windowSize + maxTau)) < 0.003f)
        return std::nullopt;

    difference.assign ((size_t) maxTau + 2, 1.0f);
    auto runningSum = 0.0f;

    for (int tau = 1; tau <= maxTau; ++tau)
    {
        auto sum = 0.0f;

        for (int j = 0; j < windowSize; ++j)
        {
            const auto delta = x[j] - x[j + tau];
            sum += delta * delta;
        }

        runningSum += sum;
        difference[(size_t) tau] = runningSum > 0.0f ? sum * (float) tau / runningSum : 1.0f;
    }

    constexpr float threshold = 0.15f;

    for (int tau = minTau; tau < maxTau; ++tau)
    {
        if (difference[(size_t) tau] >= threshold)
            continue;

        while (tau + 1 < maxTau && difference[(size_t) tau + 1] < difference[(size_t) tau])
            ++tau;

        const auto before = difference[(size_t) tau - 1];
        const auto at = difference[(size_t) tau];
        const auto after = difference[(size_t) tau + 1];
        const auto denominator = before + after - 2.0f * at;
        const auto shift = std::abs (denominator) > 1.0e-9f ? 0.5f * (before - after) / denominator : 0.0f;

        return rate / ((float) tau + shift);
    }

    return std::nullopt;
}
