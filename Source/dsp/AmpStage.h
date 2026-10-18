#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"
#include "AmpModels.h"

// Tube-style bass head: preamp gain stages, model voicing, tone stack and power amp
// with sag, all 2x oversampled; then the cabinet and a clean DI blended in.
class AmpStage
{
public:
    void bind (juce::AudioProcessorValueTreeState& state)
    {
        model   = rawParam (state, ParamIds::ampModel);
        cab     = rawParam (state, ParamIds::ampCab);
        gain    = rawParam (state, ParamIds::ampGain);
        bass    = rawParam (state, ParamIds::ampBass);
        mid     = rawParam (state, ParamIds::ampMid);
        midFreq = rawParam (state, ParamIds::ampMidFreq);
        treble  = rawParam (state, ParamIds::ampTreble);
        master  = rawParam (state, ParamIds::ampMaster);
        blend   = rawParam (state, ParamIds::ampBlend);
    }

    void prepare (double sampleRate, int maxBlockSize)
    {
        baseRate = sampleRate;
        oversampling.initProcessing ((size_t) maxBlockSize);
        oversampling.reset();
        oversampledRate = sampleRate * (double) oversampling.getOversamplingFactor();

        const juce::dsp::ProcessSpec oversampledSpec { oversampledRate,
                                                       (juce::uint32) ((size_t) maxBlockSize * oversampling.getOversamplingFactor()), 1 };
        const juce::dsp::ProcessSpec baseSpec { sampleRate, (juce::uint32) maxBlockSize, 1 };

        // Coefficients first: IIR filters size their state from the filter order when prepared.
        currentModel = currentCab = -1;
        lastBass = lastMid = lastMidFreq = lastTreble = std::numeric_limits<float>::quiet_NaN();
        updateModel();
        updateToneStack();

        crossoverLow.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        crossoverHigh.setType (juce::dsp::StateVariableTPTFilterType::highpass);
        crossoverLow.prepare (oversampledSpec);
        crossoverHigh.prepare (oversampledSpec);

        const auto coupling = juce::dsp::IIR::ArrayCoefficients<float>::makeFirstOrderHighPass (oversampledRate, 15.0f);
        *firstCoupling.coefficients = coupling;
        *secondCoupling.coefficients = coupling;

        for (auto* filter : { &firstCoupling, &secondCoupling, &bassFilter, &midFilter, &trebleFilter })
            filter->prepare (oversampledSpec);

        for (auto& filter : voicing)
            filter.prepare (oversampledSpec);

        for (auto& filter : cabinet)
            filter.prepare (baseSpec);

        diBuffer.assign ((size_t) maxBlockSize, 0.0f);

        sagAttack = timeCoefficient (0.01f, oversampledRate);
        sagRelease = timeCoefficient (0.15f, oversampledRate);
        sagEnvelope = 0.0f;

        preGain.reset (oversampledRate, 0.05);
        masterGain.reset (sampleRate, 0.05);
        diBlend.reset (sampleRate, 0.05);
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        updateModel();
        updateToneStack();

        const auto& amp = ampModels[(size_t) currentModel];
        const auto driveDb = juce::jmap (gain->load() / 10.0f, amp.minDriveDb, amp.maxDriveDb);
        preGain.setTargetValue (juce::Decibels::decibelsToGain (driveDb));
        masterGain.setTargetValue (juce::Decibels::decibelsToGain (master->load() + amp.outputTrimDb));
        diBlend.setTargetValue (cabIsOff() ? 0.0f : blend->load());

        const auto numSamples = monoBlock.getNumSamples();
        std::copy_n (monoBlock.getChannelPointer (0), numSamples, diBuffer.begin());

        auto upsampled = oversampling.processSamplesUp (monoBlock);
        auto* data = upsampled.getChannelPointer (0);
        const auto biasOffset = std::tanh (amp.bias);

        auto triode = [&amp, biasOffset] (float x) { return std::tanh (x + amp.bias) - biasOffset; };

        for (size_t i = 0; i < upsampled.getNumSamples(); ++i)
        {
            auto x = data[i];
            auto cleanLows = 0.0f;

            if (amp.crossoverHz > 0.0f)
            {
                cleanLows = crossoverLow.processSample (0, x);
                x = crossoverHigh.processSample (0, x);
            }

            const auto drive = preGain.getNextValue();
            x = firstCoupling.processSample (triode (x * drive));

            if (amp.secondStageGain > 0.0f)
                x = secondCoupling.processSample (triode (x * amp.secondStageGain)) / amp.secondStageGain;

            x = x / std::sqrt (drive) + cleanLows;

            for (auto& filter : voicing)
                x = filter.processSample (x);

            x = trebleFilter.processSample (midFilter.processSample (bassFilter.processSample (x)));

            const auto level = std::abs (x);
            sagEnvelope = level + (level > sagEnvelope ? sagAttack : sagRelease) * (sagEnvelope - level);
            x /= 1.0f + amp.sag * sagEnvelope;

            data[i] = std::tanh (amp.powerDrive * x) / amp.powerDrive;
        }

        oversampling.processSamplesDown (monoBlock);

        auto* output = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < numSamples; ++i)
        {
            auto x = output[i];

            for (auto& filter : cabinet)
                x = filter.processSample (x);

            const auto amount = diBlend.getNextValue();
            output[i] = (x * (1.0f - amount) + diBuffer[i] * amount) * masterGain.getNextValue();
        }
    }

private:
    bool cabIsOff() const { return currentCab == 0; }

    void updateModel()
    {
        const auto modelIndex = juce::jlimit (0, (int) ampModels.size() - 1, juce::roundToInt (model->load()));
        const auto cabIndex = juce::jlimit (0, (int) cabModels.size() - 1, juce::roundToInt (cab->load()));

        if (modelIndex != currentModel)
        {
            currentModel = modelIndex;
            const auto& amp = ampModels[(size_t) modelIndex];

            for (size_t band = 0; band < voicing.size(); ++band)
                *voicing[band].coefficients = designBand (amp.voicing[band], oversampledRate);

            if (amp.crossoverHz > 0.0f)
            {
                crossoverLow.setCutoffFrequency (amp.crossoverHz);
                crossoverHigh.setCutoffFrequency (amp.crossoverHz);
            }
        }

        if (cabIndex != currentCab)
        {
            currentCab = cabIndex;

            for (size_t band = 0; band < cabinet.size(); ++band)
                *cabinet[band].coefficients = designBand (cabModels[(size_t) cabIndex].bands[band], baseRate);
        }
    }

    void updateToneStack()
    {
        using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
        using juce::Decibels;
        using juce::exactlyEqual;

        const auto bassDb = bass->load(), midDb = mid->load(), frequency = midFreq->load(), trebleDb = treble->load();

        if (! exactlyEqual (bassDb, lastBass))
        {
            *bassFilter.coefficients = Coeffs::makeLowShelf (oversampledRate, 80.0f, 0.7f, Decibels::decibelsToGain (bassDb));
            lastBass = bassDb;
        }

        if (! exactlyEqual (midDb, lastMid) || ! exactlyEqual (frequency, lastMidFreq))
        {
            *midFilter.coefficients = Coeffs::makePeakFilter (oversampledRate, frequency, 0.8f, Decibels::decibelsToGain (midDb));
            lastMid = midDb;
            lastMidFreq = frequency;
        }

        if (! exactlyEqual (trebleDb, lastTreble))
        {
            *trebleFilter.coefficients = Coeffs::makeHighShelf (oversampledRate, 3000.0f, 0.7f, Decibels::decibelsToGain (trebleDb));
            lastTreble = trebleDb;
        }
    }

    std::atomic<float>* model = nullptr;
    std::atomic<float>* cab = nullptr;
    std::atomic<float>* gain = nullptr;
    std::atomic<float>* bass = nullptr;
    std::atomic<float>* mid = nullptr;
    std::atomic<float>* midFreq = nullptr;
    std::atomic<float>* treble = nullptr;
    std::atomic<float>* master = nullptr;
    std::atomic<float>* blend = nullptr;

    double baseRate = 44100.0, oversampledRate = 88200.0;
    int currentModel = -1, currentCab = -1;
    float lastBass = 0.0f, lastMid = 0.0f, lastMidFreq = 0.0f, lastTreble = 0.0f;

    juce::dsp::Oversampling<float> oversampling { 1, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true };
    juce::dsp::StateVariableTPTFilter<float> crossoverLow, crossoverHigh;
    juce::dsp::IIR::Filter<float> firstCoupling, secondCoupling, bassFilter, midFilter, trebleFilter;
    std::array<juce::dsp::IIR::Filter<float>, 3> voicing;
    std::array<juce::dsp::IIR::Filter<float>, 6> cabinet;

    std::vector<float> diBuffer;
    float sagAttack = 0.0f, sagRelease = 0.0f, sagEnvelope = 0.0f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> preGain { 1.0f }, masterGain { 1.0f };
    juce::SmoothedValue<float> diBlend;
};
