#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Feed-forward peak compressor with a 6 dB soft knee. No look-ahead, so no added latency.
class Compressor
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        enabled   = rawParam (state, ParamIds::compOn);
        threshold = rawParam (state, ParamIds::compThreshold);
        ratio     = rawParam (state, ParamIds::compRatio);
        attack    = rawParam (state, ParamIds::compAttack);
        release   = rawParam (state, ParamIds::compRelease);
        makeup    = rawParam (state, ParamIds::compMakeup);
    }

    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        envelopeDb = 0.0f;
        active.reset (sampleRate, 0.05);
        active.setCurrentAndTargetValue (0.0f);
        makeupDb.reset (sampleRate, 0.05);
        gainReductionDb.store (0.0f);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        active.setTargetValue (on ? 1.0f : 0.0f);
        makeupDb.setTargetValue (makeup->load());

        if (! on && ! active.isSmoothing())
        {
            envelopeDb = 0.0f;
            gainReductionDb.store (0.0f);
            return;
        }

        const auto thresholdDb = threshold->load();
        const auto slope = 1.0f / ratio->load() - 1.0f;
        const auto attackCoeff = timeCoefficient (attack->load() * 0.001f, sampleRate);
        const auto releaseCoeff = timeCoefficient (release->load() * 0.001f, sampleRate);
        auto* data = monoBlock.getChannelPointer (0);
        auto maxReduction = 0.0f;

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto inputDb = juce::Decibels::gainToDecibels (std::abs (data[i]), -120.0f);
            const auto targetDb = staticGain (inputDb - thresholdDb, slope);
            const auto coeff = targetDb < envelopeDb ? attackCoeff : releaseCoeff;
            envelopeDb = targetDb + coeff * (envelopeDb - targetDb);
            maxReduction = juce::jmin (maxReduction, envelopeDb);

            const auto totalDb = active.getNextValue() * (envelopeDb + makeupDb.getNextValue());
            data[i] *= std::exp (totalDb * decibelsToNeper);
        }

        gainReductionDb.store (on ? -maxReduction : 0.0f);
    }

    float getGainReductionDb() const { return gainReductionDb.load(); }

private:
    static float staticGain (float overDb, float slope)
    {
        if (overDb <= -kneeDb * 0.5f)
            return 0.0f;

        if (overDb < kneeDb * 0.5f)
        {
            const auto x = overDb + kneeDb * 0.5f;
            return slope * x * x / (2.0f * kneeDb);
        }

        return slope * overDb;
    }

    static constexpr float kneeDb = 6.0f;
    static constexpr float decibelsToNeper = 0.11512925f;

    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* threshold = nullptr;
    std::atomic<float>* ratio = nullptr;
    std::atomic<float>* attack = nullptr;
    std::atomic<float>* release = nullptr;
    std::atomic<float>* makeup = nullptr;

    double sampleRate = 44100.0;
    float envelopeDb = 0.0f;
    juce::SmoothedValue<float> active, makeupDb;
    std::atomic<float> gainReductionDb { 0.0f };
};
