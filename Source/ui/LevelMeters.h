#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "ToggleSwitch.h"

// Input and output peak meters with clip LEDs, the compressor gain reduction, and the
// output limiter switch.
class LevelMeters : public juce::Component, public juce::SettableTooltipClient, private juce::Timer
{
public:
    explicit LevelMeters (BassAmpProcessor&);
    ~LevelMeters() override;

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void resized() override;

private:
    struct Meter
    {
        float levelDb = -100.0f;
        double clipUntil = 0.0;
    };

    void timerCallback() override;
    void update (Meter&, float peak, float clipThreshold, double now);
    void drawMeter (juce::Graphics&, juce::Rectangle<int> area, const Meter&, double now) const;
    void drawGainReduction (juce::Graphics&, juce::Rectangle<int> area) const;
    void drawSegments (juce::Graphics&, juce::Rectangle<int> area, float amount, bool isLevel) const;

    BassAmpProcessor& processor;
    Meter input, output;
    float gainReductionDb = 0.0f;

    ToggleSwitch limiterButton { "LIMIT" };
    juce::AudioProcessorValueTreeState::ButtonAttachment limiterAttachment;
};
