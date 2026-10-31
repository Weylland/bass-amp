#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"
#include "AmpModels.h"
#include "NamModel.h"

// Bass head: preamp gain stages, model voicing, tone stack, contour and switches, and power
// amp with sag, all 2x oversampled, or a NAM capture followed by the tone stack; then the
// cabinet (filters or an impulse response) and a clean DI blended in, and the low and high cuts.
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
        contour = rawParam (state, ParamIds::ampContour);
        deep    = rawParam (state, ParamIds::ampDeep);
        bright  = rawParam (state, ParamIds::ampBright);
        lowCut  = rawParam (state, ParamIds::ampLowCut);
        highCut = rawParam (state, ParamIds::ampHighCut);
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

        passiveStack.prepare (oversampledRate);

        // Coefficients first: IIR filters size their state from the filter order when prepared.
        currentModel = currentCab = -1;
        lastTone.fill (std::numeric_limits<float>::quiet_NaN());
        lastCharacter.fill (std::numeric_limits<float>::quiet_NaN());
        updateModel();
        updateToneStack();
        updateCharacter();

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

        for (auto& filter : character)
            filter.prepare (oversampledSpec);

        for (auto& filter : cabinet)
            filter.prepare (baseSpec);

        lowCutFilter.setType (juce::dsp::StateVariableTPTFilterType::highpass);
        highCutFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);

        for (auto* filter : { &lowCutFilter, &highCutFilter })
        {
            filter->prepare (baseSpec);
            filter->reset();
        }

        convolution.prepare (baseSpec);
        convolution.reset();

        diBuffer.assign ((size_t) maxBlockSize, 0.0f);
        capture.prepare (sampleRate, maxBlockSize);

        sagAttack = timeCoefficient (0.01f, oversampledRate);
        sagRelease = timeCoefficient (0.15f, oversampledRate);
        sagEnvelope = 0.0f;

        preGain.reset (oversampledRate, 0.05);
        masterGain.reset (sampleRate, 0.05);
        diBlend.reset (sampleRate, 0.05);
    }

    // Message thread only: the file is read and resampled in the background.
    void loadImpulseResponse (const juce::File& file)
    {
        const auto valid = file.existsAsFile();

        if (valid)
            convolution.loadImpulseResponse (file, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::yes,
                                             maxImpulseResponseSamples, juce::dsp::Convolution::Normalise::yes);

        impulseResponseLoaded.store (valid);
    }

    // Message thread: reads and warms up the capture; a missing file unloads it.
    void loadCapture (const juce::File& file)
    {
        if (! file.existsAsFile() || ! capture.load (file))
            capture.unload();
    }

    double getCaptureSampleRate() const { return capture.getExpectedSampleRate(); }   // 0: none loaded, < 0: not in the file

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        updateModel();
        updateToneStack();
        updateCharacter();

        const auto& amp = ampModels[(size_t) currentModel];
        const auto driveDb = juce::jmap (gain->load() / 10.0f, amp.minDriveDb, amp.maxDriveDb);
        preGain.setTargetValue (juce::Decibels::decibelsToGain (driveDb));
        masterGain.setTargetValue (juce::Decibels::decibelsToGain (master->load() + amp.outputTrimDb));
        diBlend.setTargetValue (cabIsOff() ? 0.0f : blend->load());

        const auto numSamples = monoBlock.getNumSamples();
        std::copy_n (monoBlock.getChannelPointer (0), numSamples, diBuffer.begin());

        if (amp.neuralCapture)
        {
            auto* samples = monoBlock.getChannelPointer (0);

            for (size_t i = 0; i < numSamples; ++i)
                samples[i] *= preGain.getNextValue();

            capture.process (samples, (int) numSamples);
        }

        auto upsampled = oversampling.processSamplesUp (monoBlock);
        auto* data = upsampled.getChannelPointer (0);

        const auto isTube = amp.clipping == Clipping::tube;
        const auto shape = [isTube] (float x) { return isTube ? std::tanh (x) : solidStateClip (x); };
        const auto biasOffset = shape (amp.bias);
        const auto stage = [&shape, &amp, biasOffset] (float x) { return shape (x + amp.bias) - biasOffset; };
        const auto passive = amp.passiveToneStack.has_value();
        const auto tone = [this, passive] (float x)
        {
            for (auto& filter : voicing)
                x = filter.processSample (x);

            x = passive ? passiveStack.processSample (x)
                        : trebleFilter.processSample (midFilter.processSample (bassFilter.processSample (x)));

            for (auto& filter : character)
                x = filter.processSample (x);

            return x;
        };

        for (size_t i = 0; i < upsampled.getNumSamples(); ++i)
        {
            auto x = data[i];

            if (amp.neuralCapture)
            {
                data[i] = tone (x);
                continue;
            }

            auto cleanLows = 0.0f;

            if (amp.crossoverHz > 0.0f)
            {
                cleanLows = crossoverLow.processSample (0, x);
                x = crossoverHigh.processSample (0, x);
            }

            const auto drive = preGain.getNextValue();
            x = firstCoupling.processSample (stage (x * drive));

            if (amp.secondStageGain > 0.0f)
                x = secondCoupling.processSample (stage (x * amp.secondStageGain)) / amp.secondStageGain;

            x = tone (x / std::sqrt (drive) + cleanLows);

            const auto level = std::abs (x);
            sagEnvelope = level + (level > sagEnvelope ? sagAttack : sagRelease) * (sagEnvelope - level);
            x /= 1.0f + amp.sag * sagEnvelope;

            data[i] = shape (amp.powerDrive * x) / amp.powerDrive;
        }

        oversampling.processSamplesDown (monoBlock);

        auto* output = monoBlock.getChannelPointer (0);

        if (cabModels[(size_t) currentCab].usesImpulseResponse)
        {
            if (impulseResponseLoaded.load())
                convolution.process (juce::dsp::ProcessContextReplacing<float> (monoBlock));
        }
        else
        {
            for (size_t i = 0; i < numSamples; ++i)
                for (auto& filter : cabinet)
                    output[i] = filter.processSample (output[i]);
        }

        lowCutFilter.setCutoffFrequency (lowCut->load());
        highCutFilter.setCutoffFrequency (juce::jmin (highCut->load(), (float) baseRate * 0.45f));

        for (size_t i = 0; i < numSamples; ++i)
        {
            const auto amount = diBlend.getNextValue();
            const auto blended = (output[i] * (1.0f - amount) + diBuffer[i] * amount) * masterGain.getNextValue();
            output[i] = highCutFilter.processSample (0, lowCutFilter.processSample (0, blended));
        }
    }

private:
    static constexpr size_t maxImpulseResponseSamples = 48000;

    // Cubic soft clipper: flat-topped like transistors and op-amps, smooth below the knee.
    static float solidStateClip (float x)
    {
        const auto clipped = juce::jlimit (-1.5f, 1.5f, x);
        return clipped - clipped * clipped * clipped * (4.0f / 27.0f);
    }

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

            if (amp.passiveToneStack.has_value())
                passiveStack.setCircuit (*amp.passiveToneStack);

            lastTone.fill (std::numeric_limits<float>::quiet_NaN());
        }

        if (cabIndex != currentCab)
        {
            currentCab = cabIndex;

            for (size_t band = 0; band < cabinet.size(); ++band)
                *cabinet[band].coefficients = designBand (cabModels[(size_t) cabIndex].bands[band], baseRate);
        }
    }

    template <size_t size>
    static bool changed (std::array<float, size>& last, const std::array<float, size>& current)
    {
        for (size_t i = 0; i < last.size(); ++i)
        {
            if (! juce::exactlyEqual (last[i], current[i]))
            {
                last = current;
                return true;
            }
        }

        return false;
    }

    void updateToneStack()
    {
        using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
        using juce::Decibels;

        const std::array<float, 4> tone { bass->load(), mid->load(), midFreq->load(), treble->load() };

        if (! changed (lastTone, tone))
            return;

        const auto& amp = ampModels[(size_t) currentModel];
        const auto [bassDb, midDb, frequency, trebleDb] = tone;

        // The active filters are always designed so they keep their order for the next model switch.
        *bassFilter.coefficients = Coeffs::makeLowShelf (oversampledRate, amp.bassHz, 0.7f, Decibels::decibelsToGain (bassDb));
        *midFilter.coefficients = Coeffs::makePeakFilter (oversampledRate, frequency, amp.midQ, Decibels::decibelsToGain (midDb));
        *trebleFilter.coefficients = Coeffs::makeHighShelf (oversampledRate, amp.trebleHz, 0.7f, Decibels::decibelsToGain (trebleDb));

        if (amp.passiveToneStack.has_value())
        {
            auto potPosition = [] (float db) { return (double) juce::jmap (db, -12.0f, 12.0f, 0.0f, 1.0f); };
            passiveStack.setControls (potPosition (trebleDb), potPosition (midDb), potPosition (bassDb));
        }
    }

    // Contour scoops the mids and lifts both ends (like a pre-shape filter); Deep adds sub
    // lows with a low-mid cut, Bright adds an 8 kHz shelf.
    void updateCharacter()
    {
        const std::array<float, 3> values { contour->load(), deep->load(), bright->load() };

        if (! changed (lastCharacter, values))
            return;

        const auto [amount, deepOn, brightOn] = values;

        const std::array<EqBand, 4> bands {{
            { EqType::lowShelf, 50.0f, 0.7f, 5.0f * amount + 3.0f * deepOn },
            { EqType::peak, 380.0f, 0.7f, -12.0f * amount },
            { EqType::peak, 500.0f, 0.8f, -6.0f * deepOn },
            { EqType::highShelf, 8000.0f, 0.7f, 6.0f * amount + 6.0f * brightOn },
        }};

        for (size_t band = 0; band < character.size(); ++band)
            *character[band].coefficients = designBand (bands[band], oversampledRate);
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
    std::atomic<float>* contour = nullptr;
    std::atomic<float>* deep = nullptr;
    std::atomic<float>* bright = nullptr;
    std::atomic<float>* lowCut = nullptr;
    std::atomic<float>* highCut = nullptr;

    double baseRate = 44100.0, oversampledRate = 88200.0;
    int currentModel = -1, currentCab = -1;
    std::array<float, 4> lastTone {};
    std::array<float, 3> lastCharacter {};

    juce::dsp::Oversampling<float> oversampling { 1, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true };
    juce::dsp::StateVariableTPTFilter<float> crossoverLow, crossoverHigh;
    juce::dsp::IIR::Filter<float> firstCoupling, secondCoupling, bassFilter, midFilter, trebleFilter;
    PassiveToneStack passiveStack;
    std::array<juce::dsp::IIR::Filter<float>, 4> voicing;
    std::array<juce::dsp::IIR::Filter<float>, 4> character;
    std::array<juce::dsp::IIR::Filter<float>, 6> cabinet;

    juce::dsp::StateVariableTPTFilter<float> lowCutFilter, highCutFilter;
    juce::dsp::Convolution convolution;
    std::atomic<bool> impulseResponseLoaded { false };
    NamModel capture;

    std::vector<float> diBuffer;
    float sagAttack = 0.0f, sagRelease = 0.0f, sagEnvelope = 0.0f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> preGain { 1.0f }, masterGain { 1.0f };
    juce::SmoothedValue<float> diBlend;
};
