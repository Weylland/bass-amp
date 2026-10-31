#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

#include <complex>

// One-touch switches after the EQ, on even when the EQ is off: a fixed low cut (80 Hz) and
// high cut (5 kHz), and kills that remove the lows, mids or highs, split at 250 Hz and
// 2.5 kHz like a DJ isolator. The bands are Linkwitz-Riley splits, so with nothing killed
// they add back up flat. Every switch fades in 20 ms, without clicks.
class QuickCuts
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        lowCutOn  = rawParam (state, ParamIds::cutLow);
        highCutOn = rawParam (state, ParamIds::cutHigh);
        killLowOn  = rawParam (state, ParamIds::killLow);
        killMidOn  = rawParam (state, ParamIds::killMid);
        killHighOn = rawParam (state, ParamIds::killHigh);
    }

    void prepare (double sampleRate)
    {
        const juce::dsp::ProcessSpec spec { sampleRate, 512, 1 };
        using Type = juce::dsp::LinkwitzRileyFilterType;

        for (auto [filter, type, frequency] : { std::tuple { &lowCut, Type::highpass, lowCutHz },
                                                std::tuple { &highCut, Type::lowpass, highCutHz },
                                                std::tuple { &lowSplit, Type::lowpass, lowSplitHz },
                                                std::tuple { &highSplit, Type::lowpass, highSplitHz },
                                                std::tuple { &lowBandAllpass, Type::allpass, highSplitHz } })
        {
            filter->setType (type);
            filter->setCutoffFrequency (frequency);
            filter->prepare (spec);
            filter->reset();
        }

        for (auto* amount : { &lowCutAmount, &highCutAmount, &lowGain, &midGain, &highGain })
            amount->reset (sampleRate, 0.02);

        lowGain.setCurrentAndTargetValue (1.0f);
        midGain.setCurrentAndTargetValue (1.0f);
        highGain.setCurrentAndTargetValue (1.0f);
        lowCutAmount.setCurrentAndTargetValue (isOn (lowCutOn) ? 1.0f : 0.0f);
        highCutAmount.setCurrentAndTargetValue (isOn (highCutOn) ? 1.0f : 0.0f);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        lowCutAmount.setTargetValue (isOn (lowCutOn) ? 1.0f : 0.0f);
        highCutAmount.setTargetValue (isOn (highCutOn) ? 1.0f : 0.0f);
        lowGain.setTargetValue (isOn (killLowOn) ? 0.0f : 1.0f);
        midGain.setTargetValue (isOn (killMidOn) ? 0.0f : 1.0f);
        highGain.setTargetValue (isOn (killHighOn) ? 0.0f : 1.0f);

        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            auto low = 0.0f, rest = 0.0f, mid = 0.0f, high = 0.0f;
            lowSplit.processSample (0, data[i], low, rest);
            highSplit.processSample (0, rest, mid, high);
            low = lowBandAllpass.processSample (0, low);

            auto x = low * lowGain.getNextValue() + mid * midGain.getNextValue() + high * highGain.getNextValue();

            const auto lowCutMix = lowCutAmount.getNextValue();
            x += lowCutMix * (lowCut.processSample (0, x) - x);

            const auto highCutMix = highCutAmount.getNextValue();
            x += highCutMix * (highCut.processSample (0, x) - x);

            data[i] = x;
        }
    }

    static bool anyOn (juce::AudioProcessorValueTreeState& state)
    {
        for (const auto* paramId : { ParamIds::cutLow, ParamIds::cutHigh, ParamIds::killLow, ParamIds::killMid, ParamIds::killHigh })
            if (state.getRawParameterValue (paramId)->load() > 0.5f)
                return true;

        return false;
    }

    // The response of the switches as they are set, for the EQ screen.
    static float magnitudeDb (juce::AudioProcessorValueTreeState& state, float frequency, double sampleRate)
    {
        using Complex = std::complex<double>;
        const auto on = [&state] (const char* paramId) { return state.getRawParameterValue (paramId)->load() > 0.5f; };
        const auto warped = [frequency, sampleRate] (float cutoff)
        {
            return Complex (0.0, std::tan (juce::MathConstants<double>::pi * juce::jmin ((double) frequency, 0.49 * sampleRate) / sampleRate)
                                     / std::tan (juce::MathConstants<double>::pi * cutoff / sampleRate));
        };
        const auto denominator = [&] (float cutoff)
        {
            const auto s = warped (cutoff);
            return s * s + std::sqrt (2.0) * s + 1.0;
        };
        const auto lowpass = [&] (float cutoff) { const auto d = denominator (cutoff); return 1.0 / (d * d); };
        const auto highpass = [&] (float cutoff)
        {
            const auto s = warped (cutoff);
            const auto d = denominator (cutoff);
            return (s * s * s * s) / (d * d);
        };

        auto response = (on (ParamIds::killLow) ? 0.0 : 1.0) * lowpass (lowSplitHz) * (lowpass (highSplitHz) + highpass (highSplitHz))
                       + (on (ParamIds::killMid) ? 0.0 : 1.0) * highpass (lowSplitHz) * lowpass (highSplitHz)
                       + (on (ParamIds::killHigh) ? 0.0 : 1.0) * highpass (lowSplitHz) * highpass (highSplitHz);

        if (on (ParamIds::cutLow))
            response *= highpass (lowCutHz);

        if (on (ParamIds::cutHigh))
            response *= lowpass (highCutHz);

        return juce::Decibels::gainToDecibels ((float) std::abs (response), -120.0f);
    }

private:
    static constexpr float lowCutHz = 80.0f, highCutHz = 5000.0f, lowSplitHz = 250.0f, highSplitHz = 2500.0f;

    std::atomic<float>* lowCutOn = nullptr;
    std::atomic<float>* highCutOn = nullptr;
    std::atomic<float>* killLowOn = nullptr;
    std::atomic<float>* killMidOn = nullptr;
    std::atomic<float>* killHighOn = nullptr;

    juce::dsp::LinkwitzRileyFilter<float> lowCut, highCut, lowSplit, highSplit, lowBandAllpass;
    juce::SmoothedValue<float> lowCutAmount, highCutAmount, lowGain, midGain, highGain;
};
