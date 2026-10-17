#pragma once

#include <juce_dsp/juce_dsp.h>

// Preamp saturation, 3-band tone stack, fixed cabinet filtering and master volume.
class AmpStage
{
public:
    void prepare (const juce::dsp::ProcessSpec& monoSpec)
    {
        sampleRate = monoSpec.sampleRate;

        using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
        *cabHighPass.coefficients = Coeffs::makeHighPass (sampleRate, 35.0f, 0.707f);
        *cabLowPass.coefficients  = Coeffs::makeLowPass (sampleRate, 4500.0f, 0.707f);

        lastBass = lastMid = lastTreble = std::numeric_limits<float>::quiet_NaN();
        setToneStack (0.0f, 0.0f, 0.0f);

        for (auto* filter : { &bass, &mid, &treble, &cabHighPass, &cabLowPass })
            filter->prepare (monoSpec);

        preGain.reset (sampleRate, 0.05);
        master.reset (sampleRate, 0.05);
    }

    void setParameters (float gain, float bassDb, float midDb, float trebleDb, float masterDb)
    {
        preGain.setTargetValue (juce::Decibels::decibelsToGain (gain * 2.4f));
        master.setTargetValue (juce::Decibels::decibelsToGain (masterDb));
        setToneStack (bassDb, midDb, trebleDb);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            auto x = std::tanh (preGain.getNextValue() * data[i]);
            x = bass.processSample (x);
            x = mid.processSample (x);
            x = treble.processSample (x);
            x = cabHighPass.processSample (x);
            x = cabLowPass.processSample (x);
            data[i] = x * master.getNextValue();
        }
    }

private:
    void setToneStack (float bassDb, float midDb, float trebleDb)
    {
        using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
        using juce::Decibels;

        if (bassDb != lastBass)
        {
            *bass.coefficients = Coeffs::makeLowShelf (sampleRate, 100.0f, 0.707f, Decibels::decibelsToGain (bassDb));
            lastBass = bassDb;
        }

        if (midDb != lastMid)
        {
            *mid.coefficients = Coeffs::makePeakFilter (sampleRate, 600.0f, 0.8f, Decibels::decibelsToGain (midDb));
            lastMid = midDb;
        }

        if (trebleDb != lastTreble)
        {
            *treble.coefficients = Coeffs::makeHighShelf (sampleRate, 2500.0f, 0.707f, Decibels::decibelsToGain (trebleDb));
            lastTreble = trebleDb;
        }
    }

    double sampleRate = 44100.0;
    float lastBass = 0.0f, lastMid = 0.0f, lastTreble = 0.0f;

    juce::dsp::IIR::Filter<float> bass, mid, treble, cabHighPass, cabLowPass;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> preGain { 1.0f }, master { 1.0f };
};
