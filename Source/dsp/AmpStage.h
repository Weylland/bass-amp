#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"

// Preamp saturation, 3-band tone stack, fixed cabinet filtering and master volume.
class AmpStage
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        gain   = rawParam (state, ParamIds::ampGain);
        bass   = rawParam (state, ParamIds::ampBass);
        mid    = rawParam (state, ParamIds::ampMid);
        treble = rawParam (state, ParamIds::ampTreble);
        master = rawParam (state, ParamIds::ampMaster);
    }

    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        sampleRate = monoSpec.sampleRate;

        using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
        *cabHighPass.coefficients = Coeffs::makeHighPass (sampleRate, 35.0f, 0.707f);
        *cabLowPass.coefficients  = Coeffs::makeLowPass (sampleRate, 4500.0f, 0.707f);

        lastBass = lastMid = lastTreble = std::numeric_limits<float>::quiet_NaN();
        setToneStack (0.0f, 0.0f, 0.0f);

        for (auto* filter : { &bassFilter, &midFilter, &trebleFilter, &cabHighPass, &cabLowPass })
            filter->prepare (monoSpec);

        preGain.reset (sampleRate, 0.05);
        masterGain.reset (sampleRate, 0.05);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        preGain.setTargetValue (juce::Decibels::decibelsToGain (gain->load() * 2.4f));
        masterGain.setTargetValue (juce::Decibels::decibelsToGain (master->load()));
        setToneStack (bass->load(), mid->load(), treble->load());

        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            auto x = std::tanh (preGain.getNextValue() * data[i]);
            x = bassFilter.processSample (x);
            x = midFilter.processSample (x);
            x = trebleFilter.processSample (x);
            x = cabHighPass.processSample (x);
            x = cabLowPass.processSample (x);
            data[i] = x * masterGain.getNextValue();
        }
    }

private:
    void setToneStack (float bassDb, float midDb, float trebleDb)
    {
        using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
        using juce::Decibels;

        if (bassDb != lastBass)
        {
            *bassFilter.coefficients = Coeffs::makeLowShelf (sampleRate, 100.0f, 0.707f, Decibels::decibelsToGain (bassDb));
            lastBass = bassDb;
        }

        if (midDb != lastMid)
        {
            *midFilter.coefficients = Coeffs::makePeakFilter (sampleRate, 600.0f, 0.8f, Decibels::decibelsToGain (midDb));
            lastMid = midDb;
        }

        if (trebleDb != lastTreble)
        {
            *trebleFilter.coefficients = Coeffs::makeHighShelf (sampleRate, 2500.0f, 0.707f, Decibels::decibelsToGain (trebleDb));
            lastTreble = trebleDb;
        }
    }

    std::atomic<float>* gain = nullptr;
    std::atomic<float>* bass = nullptr;
    std::atomic<float>* mid = nullptr;
    std::atomic<float>* treble = nullptr;
    std::atomic<float>* master = nullptr;

    double sampleRate = 44100.0;
    float lastBass = 0.0f, lastMid = 0.0f, lastTreble = 0.0f;

    juce::dsp::IIR::Filter<float> bassFilter, midFilter, trebleFilter, cabHighPass, cabLowPass;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> preGain { 1.0f }, masterGain { 1.0f };
};
