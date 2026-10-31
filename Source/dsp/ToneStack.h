#pragma once

#include <juce_dsp/juce_dsp.h>

#include <complex>

// Component values of the classic passive treble/middle/bass network.
struct ToneStackCircuit
{
    double r1, r2, r3, r4, c1, c2, c3;
};

// Passive tone stack after D. T. Yeh's analysis ("Discretization of the '59 Fender Bassman
// Tone Stack", DAFx 2006), as used in Guitarix: a third-order analog transfer function,
// bilinear-transformed. The controls interact like the real circuit. Normalised so that
// the response peaks at 0 dB with every control at noon.
class PassiveToneStack
{
public:
    void setCircuit (const ToneStackCircuit& newCircuit)
    {
        circuit = newCircuit;
        const auto noon = analog (0.5, 0.5, 0.5);
        auto peak = 0.0;

        for (auto frequency = 20.0; frequency < 20000.0; frequency *= 1.05)
            peak = std::max (peak, std::abs (noon.response (frequency)));

        makeup = 1.0 / peak;
    }

    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        std::fill (std::begin (state), std::end (state), 0.0);
    }

    // treble, middle and bass are pot positions from 0 to 1.
    void setControls (double treble, double middle, double bass)
    {
        const auto h = analog (treble, middle, bass);
        const auto c = 2.0 * sampleRate;

        // s = c (1 - z^-1) / (1 + z^-1), then multiply through by (1 + z^-1)^3.
        std::array<double, 4> b {}, a {};

        for (int power = 0; power <= 3; ++power)
        {
            const auto terms = expand (power, 3 - power);
            const auto scale = std::pow (c, power);

            for (size_t k = 0; k < 4; ++k)
            {
                b[k] += h.b[(size_t) power] * scale * terms[k];
                a[k] += h.a[(size_t) power] * scale * terms[k];
            }
        }

        for (size_t k = 0; k < 4; ++k)
        {
            numerator[k] = b[k] / a[0] * makeup;
            denominator[k] = a[k] / a[0];
        }
    }

    float processSample (float input)
    {
        const auto x = (double) input;
        const auto y = numerator[0] * x + state[0];
        state[0] = numerator[1] * x - denominator[1] * y + state[1];
        state[1] = numerator[2] * x - denominator[2] * y + state[2];
        state[2] = numerator[3] * x - denominator[3] * y;
        return (float) y;
    }

private:
    struct Analog
    {
        std::array<double, 4> b, a;   // coefficients of s^0 .. s^3

        std::complex<double> response (double frequency) const
        {
            const std::complex<double> s (0.0, juce::MathConstants<double>::twoPi * frequency);
            return (((b[3] * s + b[2]) * s + b[1]) * s + b[0]) / (((a[3] * s + a[2]) * s + a[1]) * s + a[0]);
        }
    };

    Analog analog (double t, double m, double bass) const
    {
        const auto [R1, R2, R3, R4, C1, C2, C3] = circuit;
        const auto l = std::exp ((bass - 1.0) * 3.4);   // the bass pot is log taper

        Analog h;
        h.b[0] = 0.0;
        h.b[1] = t * C1 * R1 + m * C3 * R3 + l * (C1 * R2 + C2 * R2) + (C1 * R3 + C2 * R3);
        h.b[2] = t * (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4) - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3)
               + m * (C1 * C3 * R1 * R3 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3)
               + l * (C1 * C2 * R1 * R2 + C1 * C2 * R2 * R4 + C1 * C3 * R2 * R4)
               + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3)
               + (C1 * C2 * R1 * R3 + C1 * C2 * R3 * R4 + C1 * C3 * R3 * R4);
        h.b[3] = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4)
               - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4)
               + m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4)
               + t * C1 * C2 * C3 * R1 * R3 * R4 - t * m * C1 * C2 * C3 * R1 * R3 * R4
               + t * l * C1 * C2 * C3 * R1 * R2 * R4;

        h.a[0] = 1.0;
        h.a[1] = (C1 * R1 + C1 * R3 + C2 * R3 + C2 * R4 + C3 * R4) + m * C3 * R3 + l * (C1 * R2 + C2 * R2);
        h.a[2] = m * (C1 * C3 * R1 * R3 - C2 * C3 * R3 * R4 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3)
               + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3)
               - m * m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3)
               + l * (C1 * C2 * R2 * R4 + C1 * C2 * R1 * R2 + C1 * C3 * R2 * R4 + C2 * C3 * R2 * R4)
               + (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 + C1 * C2 * R3 * R4 + C1 * C2 * R1 * R3 + C1 * C3 * R3 * R4 + C2 * C3 * R3 * R4);
        h.a[3] = l * m * (C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4)
               - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4)
               + m * (C1 * C2 * C3 * R3 * R3 * R4 + C1 * C2 * C3 * R1 * R3 * R3 - C1 * C2 * C3 * R1 * R3 * R4)
               + l * C1 * C2 * C3 * R1 * R2 * R4
               + C1 * C2 * C3 * R1 * R3 * R4;
        return h;
    }

    // Coefficients of (1 - z^-1)^minus * (1 + z^-1)^plus, in powers of z^-1.
    static std::array<double, 4> expand (int minus, int plus)
    {
        std::array<double, 4> poly { 1.0, 0.0, 0.0, 0.0 };
        auto degree = 0;

        auto multiply = [&poly, &degree] (double sign)
        {
            for (auto k = degree + 1; k > 0; --k)
                poly[(size_t) k] += sign * poly[(size_t) k - 1];

            ++degree;
        };

        for (int i = 0; i < minus; ++i)
            multiply (-1.0);

        for (int i = 0; i < plus; ++i)
            multiply (1.0);

        return poly;
    }

    ToneStackCircuit circuit { 250e3, 1e6, 25e3, 56e3, 250e-12, 20e-9, 20e-9 };
    double sampleRate = 44100.0, makeup = 1.0;
    std::array<double, 4> numerator { 1.0, 0.0, 0.0, 0.0 }, denominator { 1.0, 0.0, 0.0, 0.0 };
    double state[3] {};
};
