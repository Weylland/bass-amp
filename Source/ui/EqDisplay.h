#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "../dsp/Equalizer.h"
#include "../dsp/SpectrumTap.h"

// EQ screen: the output spectrum behind the EQ response curve. Basic mode shows one fader
// per graphic band; advanced mode has draggable nodes (drag for frequency and gain,
// mouse wheel for width, double-click to reset). Touching a fader or a node switches the EQ on.
// The curve includes the quick cuts, which work even with the EQ off.
// Its tooltip explains the node under the mouse.
class EqDisplay : public juce::Component, public juce::TooltipClient
{
public:
    EqDisplay (juce::AudioProcessorValueTreeState&, SpectrumTap&);

    void setMode (bool advanced, bool active);
    void updateSpectrum();

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseMove (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

    juce::String getTooltip() override;

private:
    struct Node
    {
        const char* name;
        const char* frequencyId;
        const char* gainId;     // nullptr for the cut filters
        const char* qId;        // nullptr when the width is fixed
        juce::Colour colour;
    };

    static constexpr int fftOrder = 12;
    static constexpr int fftSize = 1 << fftOrder;

    juce::Rectangle<float> plotArea() const;
    float xForFrequency (float frequency) const;
    float frequencyForX (float x) const;
    float yForDb (float db) const;
    float dbForY (float y) const;
    std::vector<std::array<float, 6>> currentCoefficients() const;
    juce::Point<float> nodePosition (int index) const;
    int nodeAt (juce::Point<float>) const;
    float value (const char* paramId) const;
    void setValue (const char* paramId, float newValue);
    void switchOn();

    void paintGrid (juce::Graphics&, juce::Rectangle<float>);
    void paintSpectrum (juce::Graphics&, juce::Rectangle<float>);
    void paintCurve (juce::Graphics&, juce::Rectangle<float>);
    void paintNodes (juce::Graphics&);

    juce::AudioProcessorValueTreeState& state;
    SpectrumTap& tap;
    Equalizer::Sources sources;
    bool advanced = false, active = false;

    juce::OwnedArray<juce::Slider> faders;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> faderAttachments;

    std::array<Node, 6> nodes;
    int hoveredNode = -1, draggedNode = -1;

    juce::dsp::FFT fft { fftOrder };
    juce::dsp::WindowingFunction<float> window { (size_t) fftSize, juce::dsp::WindowingFunction<float>::hann };
    std::vector<float> history = std::vector<float> ((size_t) fftSize, 0.0f);
    std::vector<float> incoming = std::vector<float> ((size_t) fftSize, 0.0f);
    std::vector<float> fftData = std::vector<float> ((size_t) fftSize * 2, 0.0f);
    std::vector<float> spectrumDb = std::vector<float> ((size_t) fftSize / 2, -120.0f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqDisplay)
};
