#pragma once

#include <juce_core/juce_core.h>

#include <array>
#include <cmath>

// Tube stages modelled from their circuits, for the heads that go beyond a fixed clipping curve.
// They run at the oversampled rate and take the signal as volts at the grid.
namespace TubeCircuit
{
    // A curve sampled once and read with linear interpolation; inputs outside the range hold the
    // value at the nearest end.
    template <int size>
    struct Table
    {
        float low = 0.0f, high = 1.0f;
        std::array<float, size> values {};

        template <typename Function>
        void fill (float from, float to, Function&& function)
        {
            low = from;
            high = to;

            for (int i = 0; i < size; ++i)
                values[(size_t) i] = function (from + (to - from) * (float) i / (float) (size - 1));
        }

        float operator() (float x) const
        {
            const auto position = juce::jlimit (0.0f, (float) (size - 1), (x - low) / (high - low) * (float) (size - 1));
            const auto index = juce::jmin ((int) position, size - 2);
            const auto fraction = position - (float) index;
            return values[(size_t) index] + fraction * (values[(size_t) index + 1] - values[(size_t) index]);
        }
    };

    // A 12AX7 common-cathode stage: 100k plate load on a 300 V supply, 1.5k cathode resistor
    // bypassed. The plate current follows Koren's triode model, solved on the load line into a
    // table. Its input coupling capacitor (22 nF, from a 50k source, 1M grid leak) charges fast
    // when a loud note drives the grid past the cathode and slowly lets go: the bias shifts down
    // for a moment, so the stage compresses and clips asymmetrically after a hard attack, then
    // recovers. The output is scaled to the stage's small-signal gain, so a quiet signal comes out
    // as it went in.
    class TriodeStage
    {
    public:
        void prepare (double sampleRate)
        {
            chargeCoefficient = (float) (1.0 / (sourceOhms * couplingFarads * sampleRate));
            couplingVolts = 0.0f;
        }

        float process (float input)
        {
            const auto& curve = circuit();
            const auto beforeGrid = input - couplingVolts;
            const auto grid = gridVoltage (beforeGrid, curve.cathodeVolts);
            couplingVolts += (beforeGrid - grid) * chargeCoefficient;
            return curve.plate (grid);
        }

        float getBiasShift() const { return couplingVolts; }

    private:
        static constexpr double supplyVolts = 300.0, plateOhms = 100e3, cathodeOhms = 1.5e3;
        static constexpr double sourceOhms = 50e3, leakOhms = 1e6, conductingOhms = 2e3, couplingFarads = 22e-9;

        struct Circuit
        {
            float cathodeVolts = 0.0f;
            Table<4096> plate;
        };

        // Koren's 12AX7: mu 100, ex 1.4, kg1 1060, kp 600, kvb 300.
        static double plateCurrent (double gridToCathode, double plateToCathode)
        {
            if (plateToCathode <= 0.0)
                return 0.0;

            const auto e1 = plateToCathode / 600.0
                            * std::log1p (std::exp (600.0 * (1.0 / 100.0 + gridToCathode / std::sqrt (300.0 + plateToCathode * plateToCathode))));
            return 2.0 * std::pow (e1, 1.4) / 1060.0;
        }

        // The plate current where the tube and its load line agree.
        static double solve (double gridToCathode, double cathodeVolts, double seriesOhms)
        {
            auto low = 0.0, high = supplyVolts / seriesOhms;

            for (int i = 0; i < 60; ++i)
            {
                const auto current = (low + high) / 2.0;
                const auto tube = plateCurrent (gridToCathode, supplyVolts - current * seriesOhms - cathodeVolts);
                (tube > current ? low : high) = current;
            }

            return (low + high) / 2.0;
        }

        static const Circuit& circuit()
        {
            static const Circuit built = []
            {
                Circuit result;

                // The idle point sets the cathode voltage, which the bypass capacitor then holds.
                auto idle = 0.0;

                for (int i = 0; i < 40; ++i)
                    idle = solve (-idle * cathodeOhms, 0.0, plateOhms + cathodeOhms);

                const auto cathodeVolts = idle * cathodeOhms;
                const auto plateVolts = [cathodeVolts] (double grid)
                {
                    return supplyVolts - solve (grid - cathodeVolts, cathodeVolts, plateOhms) * plateOhms;
                };

                const auto idleVolts = plateVolts (0.0);
                const auto gain = (plateVolts (0.01) - plateVolts (-0.01)) / 0.02;

                result.cathodeVolts = (float) cathodeVolts;
                result.plate.fill (-10.0f, 4.0f, [&] (float grid) { return (float) ((plateVolts (grid) - idleVolts) / gain); });
                return result;
            }();

            return built;
        }

        // The grid follows its input until it passes the cathode; then it conducts and the
        // source resistance holds it down, with a smooth knee.
        static float gridVoltage (float input, float cathodeVolts)
        {
            constexpr auto open = (float) (leakOhms / (sourceOhms + leakOhms));
            constexpr auto conducting = (float) (conductingOhms / (sourceOhms + conductingOhms)) / open;
            constexpr auto knee = 0.3f;

            const auto grid = input * open;
            const auto excess = grid - cathodeVolts;

            if (excess <= 0.0f)
                return grid;

            return cathodeVolts + excess * conducting + (1.0f - conducting) * knee * std::tanh (excess / knee);
        }

        float chargeCoefficient = 0.0f, couplingVolts = 0.0f;
    };

    // A pair of 6550 beam tetrodes in class AB push-pull. Each tube's current rises with the
    // 3/2 power of its drive above cut-off and levels off at the plate limit; the halves cancel
    // their even harmonics and leave a little crossover around idle. Global negative feedback
    // keeps it clean until the tubes run out, the supply sags under sustained level, and the
    // output transformer's core saturates on big lows. Unity gain for quiet signals.
    class PushPull
    {
    public:
        void prepare (double sampleRate)
        {
            const auto coefficient = [sampleRate] (double seconds) { return (float) std::exp (-1.0 / (seconds * sampleRate)); };
            sagAttack = coefficient (0.005);
            sagRelease = coefficient (0.12);
            transformerCoefficient = coefficient (1.0 / (juce::MathConstants<double>::twoPi * 70.0));
            reset();
        }

        void reset()
        {
            previous = sagEnvelope = transformerLows = 0.0f;
        }

        float process (float input, float sag)
        {
            const auto& curve = transfer();
            const auto headroom = 1.0f / (1.0f + sag * sagEnvelope);
            const auto drive = ((1.0f + feedback) * input - feedback * previous) / headroom;
            auto output = headroom * curve (drive);
            previous = output;

            const auto level = std::abs (output);
            sagEnvelope = level + (level > sagEnvelope ? sagAttack : sagRelease) * (sagEnvelope - level);

            transformerLows = output + transformerCoefficient * (transformerLows - output);
            output += transformerSaturation (transformerLows) - transformerLows;
            return output;
        }

    private:
        static constexpr float feedback = 0.5f, idleBias = 0.3f, smoothing = 0.08f, plateLimit = 1.0f;

        static float tubeCurrent (float drive)
        {
            const auto above = smoothing * std::log1p (std::exp (drive / smoothing));
            const auto current = std::pow (above, 1.5f);
            return current / std::cbrt (1.0f + std::pow (current / plateLimit, 3.0f));
        }

        static const Table<4096>& transfer()
        {
            static const Table<4096> built = []
            {
                const auto pair = [] (float drive) { return tubeCurrent (idleBias + drive) - tubeCurrent (idleBias - drive); };
                const auto slope = (pair (0.001f) - pair (-0.001f)) / 0.002f;
                Table<4096> table;
                table.fill (-4.0f, 4.0f, [&] (float drive) { return pair (drive) / slope; });
                return table;
            }();

            return built;
        }

        static float transformerSaturation (float lows)
        {
            constexpr auto core = 1.5f;
            return core * std::tanh (lows / core);
        }

        float previous = 0.0f, sagEnvelope = 0.0f, transformerLows = 0.0f;
        float sagAttack = 0.0f, sagRelease = 0.0f, transformerCoefficient = 0.0f;
    };
}
