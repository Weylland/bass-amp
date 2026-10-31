#pragma once

#include <juce_dsp/juce_dsp.h>

#include "../Parameters.h"
#include "EqBand.h"

// Post-amp EQ with two modes. Basic: 9-band octave graphic EQ. Advanced: low cut, low shelf,
// two parametric mids, high shelf and high cut. Both banks run all the time and are
// crossfaded, so switching mode or bypass never clicks.
class Equalizer
{
public:
    static constexpr std::array<float, 9> graphicFrequencies { 33.0f, 63.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f };
    static constexpr float graphicQ = 1.4f;

    static constexpr int basicMode = 0;
    static constexpr int advancedMode = 1;

    using GraphicBands = std::array<EqBand, 9>;
    using ParametricBands = std::array<EqBand, 6>;

    // The parameters the bands are read from; the UI uses the same view to draw the curve.
    struct Sources
    {
        explicit Sources (juce::AudioProcessorValueTreeState& state)
        {
            using namespace ParamIds;

            for (size_t i = 0; i < graphic.size(); ++i)
                graphic[i] = rawParam (state, graphicEq[i]);

            const std::array<const char*, 12> ids { peqLowCut, peqLowFreq, peqLowGain, peqMid1Freq, peqMid1Gain, peqMid1Q,
                                                    peqMid2Freq, peqMid2Gain, peqMid2Q, peqHighFreq, peqHighGain, peqHighCut };

            for (size_t i = 0; i < parametric.size(); ++i)
                parametric[i] = rawParam (state, ids[i]);
        }

        GraphicBands graphicBands() const
        {
            GraphicBands bands;

            for (size_t i = 0; i < bands.size(); ++i)
                bands[i] = { EqBand::Type::peak, graphicFrequencies[i], graphicQ, graphic[i]->load() };

            return bands;
        }

        ParametricBands parametricBands() const
        {
            auto value = [this] (size_t index) { return parametric[index]->load(); };

            return {{
                { EqBand::Type::highPass, value (0), 0.707f, 0.0f },
                { EqBand::Type::lowShelf, value (1), 0.7f, value (2) },
                { EqBand::Type::peak, value (3), value (5), value (4) },
                { EqBand::Type::peak, value (6), value (8), value (7) },
                { EqBand::Type::highShelf, value (9), 0.7f, value (10) },
                { EqBand::Type::lowPass, value (11), 0.707f, 0.0f },
            }};
        }

        std::array<std::atomic<float>*, 9> graphic {};
        std::array<std::atomic<float>*, 12> parametric {};
    };

    template <size_t size>
    static std::array<std::array<float, 6>, size> design (const std::array<EqBand, size>& bands, double sampleRate)
    {
        std::array<std::array<float, 6>, size> coefficients;

        for (size_t i = 0; i < size; ++i)
        {
            auto band = bands[i];
            band.frequency = juce::jmin (band.frequency, (float) sampleRate * 0.45f);
            coefficients[i] = designBand (band, sampleRate);
        }

        return coefficients;
    }

    void bind (juce::AudioProcessorValueTreeState& state)
    {
        sources = std::make_unique<Sources> (state);
        enabled = rawParam (state, ParamIds::eqOn);
        mode = rawParam (state, ParamIds::eqMode);
    }

    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        const juce::dsp::ProcessSpec spec { sampleRate, 512, 1 };

        lastGraphic = {};
        lastParametric = {};
        updateCoefficients (true);

        for (auto& filter : graphic)
            filter.prepare (spec);

        for (auto& filter : parametric)
            filter.prepare (spec);

        for (auto* weight : { &graphicWeight, &parametricWeight })
        {
            weight->reset (sampleRate, 0.03);
            weight->setCurrentAndTargetValue (0.0f);
        }
    }

    void process (juce::dsp::AudioBlock<float>& monoBlock)
    {
        const auto on = isOn (enabled);
        const auto advanced = juce::roundToInt (mode->load()) == advancedMode;
        graphicWeight.setTargetValue (on && ! advanced ? 1.0f : 0.0f);
        parametricWeight.setTargetValue (on && advanced ? 1.0f : 0.0f);

        if (! on && ! graphicWeight.isSmoothing() && ! parametricWeight.isSmoothing())
            return;

        updateCoefficients (false);
        auto* data = monoBlock.getChannelPointer (0);

        for (size_t i = 0; i < monoBlock.getNumSamples(); ++i)
        {
            const auto dry = data[i];
            auto basic = dry, pro = dry;

            for (auto& filter : graphic)
                basic = filter.processSample (basic);

            for (auto& filter : parametric)
                pro = filter.processSample (pro);

            const auto wBasic = graphicWeight.getNextValue();
            const auto wPro = parametricWeight.getNextValue();
            data[i] = dry * (1.0f - wBasic - wPro) + basic * wBasic + pro * wPro;
        }
    }

private:
    template <size_t size>
    static bool changed (std::array<EqBand, size>& last, const std::array<EqBand, size>& current)
    {
        auto same = [] (const EqBand& a, const EqBand& b)
        {
            return a.type == b.type && juce::exactlyEqual (a.frequency, b.frequency)
                && juce::exactlyEqual (a.q, b.q) && juce::exactlyEqual (a.gainDb, b.gainDb);
        };

        if (std::equal (last.begin(), last.end(), current.begin(), same))
            return false;

        last = current;
        return true;
    }

    void updateCoefficients (bool force)
    {
        if (const auto bands = sources->graphicBands(); changed (lastGraphic, bands) || force)
        {
            const auto coefficients = design (bands, sampleRate);

            for (size_t i = 0; i < graphic.size(); ++i)
                *graphic[i].coefficients = coefficients[i];
        }

        if (const auto bands = sources->parametricBands(); changed (lastParametric, bands) || force)
        {
            const auto coefficients = design (bands, sampleRate);

            for (size_t i = 0; i < parametric.size(); ++i)
                *parametric[i].coefficients = coefficients[i];
        }
    }

    std::unique_ptr<Sources> sources;
    std::atomic<float>* enabled = nullptr;
    std::atomic<float>* mode = nullptr;

    double sampleRate = 44100.0;
    GraphicBands lastGraphic {};
    ParametricBands lastParametric {};
    std::array<juce::dsp::IIR::Filter<float>, 9> graphic;
    std::array<juce::dsp::IIR::Filter<float>, 6> parametric;
    juce::SmoothedValue<float> graphicWeight, parametricWeight;
};
