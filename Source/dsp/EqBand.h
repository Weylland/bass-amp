#pragma once

#include <juce_dsp/juce_dsp.h>

struct EqBand
{
    enum class Type { none, lowShelf, highShelf, peak, lowPass, highPass };

    Type type = Type::none;
    float frequency = 1000.0f;
    float q = 0.707f;
    float gainDb = 0.0f;
};

inline std::array<float, 6> designBand (const EqBand& band, double sampleRate)
{
    using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
    const auto gain = juce::Decibels::decibelsToGain (band.gainDb);

    switch (band.type)
    {
        case EqBand::Type::lowShelf:  return Coeffs::makeLowShelf (sampleRate, band.frequency, band.q, gain);
        case EqBand::Type::highShelf: return Coeffs::makeHighShelf (sampleRate, band.frequency, band.q, gain);
        case EqBand::Type::peak:      return Coeffs::makePeakFilter (sampleRate, band.frequency, band.q, gain);
        case EqBand::Type::lowPass:   return Coeffs::makeLowPass (sampleRate, band.frequency, band.q);
        case EqBand::Type::highPass:  return Coeffs::makeHighPass (sampleRate, band.frequency, band.q);
        case EqBand::Type::none:      break;
    }

    return { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
}
