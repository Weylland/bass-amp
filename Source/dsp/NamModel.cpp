#include "NamModel.h"

#include <NAM/dsp.h>
#include <NAM/get_dsp.h>

namespace
{
    // NAM captures are levelled to this loudness, like the reference NAM plugin does.
    constexpr double targetLoudnessDb = -18.0;
}

NamModel::NamModel() = default;
NamModel::~NamModel() = default;

bool NamModel::load (const juce::File& file)
{
    std::unique_ptr<nam::DSP> loaded;

    try
    {
        loaded = nam::get_dsp (std::filesystem::u8path (file.getFullPathName().toStdString()));
    }
    catch (const std::exception&)
    {
        return false;
    }

    if (loaded == nullptr)
        return false;

    double rate;
    int blockSize;
    {
        const juce::SpinLock::ScopedLockType scopedLock (lock);
        rate = sampleRate;
        blockSize = maxBlockSize;
    }

    loaded->Reset (rate, blockSize);
    const auto gain = loaded->HasLoudness() ? (float) juce::Decibels::decibelsToGain (targetLoudnessDb - loaded->GetLoudness()) : 1.0f;
    expectedSampleRate.store (loaded->GetExpectedSampleRate());

    {
        const juce::SpinLock::ScopedLockType scopedLock (lock);
        std::swap (model, loaded);
        outputGain = gain;
    }

    return true;
}

void NamModel::unload()
{
    std::unique_ptr<nam::DSP> previous;

    {
        const juce::SpinLock::ScopedLockType scopedLock (lock);
        std::swap (model, previous);
    }

    expectedSampleRate.store (0.0);
}

void NamModel::prepare (double newSampleRate, int newMaxBlockSize)
{
    const juce::SpinLock::ScopedLockType scopedLock (lock);
    sampleRate = newSampleRate;
    maxBlockSize = newMaxBlockSize;
    output.assign ((size_t) newMaxBlockSize, 0.0f);

    if (model != nullptr)
        model->Reset (sampleRate, maxBlockSize);
}

bool NamModel::process (float* samples, int numSamples)
{
    const juce::SpinLock::ScopedTryLockType scopedLock (lock);

    if (! scopedLock.isLocked() || model == nullptr)
        return false;

    for (int start = 0; start < numSamples; start += maxBlockSize)
    {
        const auto count = juce::jmin (maxBlockSize, numSamples - start);
        auto* input = samples + start;
        auto* result = output.data();
        model->process (&input, &result, count);
        juce::FloatVectorOperations::multiply (input, result, outputGain, count);
    }

    return true;
}
