#pragma once

#include <juce_dsp/juce_dsp.h>

#include <complex>

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

// Magnitude of a biquad (b0, b1, b2, a0, a1, a2) at a frequency, in dB.
inline float magnitudeDb (const std::array<float, 6>& c, double frequency, double sampleRate)
{
    const auto w = juce::MathConstants<double>::twoPi * frequency / sampleRate;
    const std::complex<double> z1 = std::polar (1.0, -w), z2 = z1 * z1;
    const auto h = ((double) c[0] + (double) c[1] * z1 + (double) c[2] * z2) / ((double) c[3] + (double) c[4] * z1 + (double) c[5] * z2);
    return (float) juce::Decibels::gainToDecibels (std::abs (h), -120.0);
}
