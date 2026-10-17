#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"

// Input and output peak meters with clip LEDs, plus the compressor gain reduction.
class LevelMeters : public juce::Component, private juce::Timer
{
public:
    explicit LevelMeters (BassAmpProcessor&);
    ~LevelMeters() override;

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;

private:
    struct Meter
    {
        float levelDb = -100.0f;
        double clipUntil = 0.0;
    };

    void timerCallback() override;
    void update (Meter&, float peak, float clipThreshold, double now);
    void drawMeter (juce::Graphics&, juce::Rectangle<int> area, const juce::String& name, const Meter&, double now) const;
    void drawGainReduction (juce::Graphics&, juce::Rectangle<int> area) const;

    BassAmpProcessor& processor;
    Meter input, output;
    float gainReductionDb = 0.0f;
};
