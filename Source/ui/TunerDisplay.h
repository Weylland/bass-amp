#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"

// Note name, cents needle and frequency, plus a button that mutes the output while tuning.
// The - and + buttons or the mouse wheel set the reference A (430 to 450 Hz), double-click
// resets it to 440 Hz; the choice is kept for every session.
class TunerDisplay : public juce::Component, public juce::SettableTooltipClient, private juce::Timer
{
public:
    explicit TunerDisplay (BassAmpProcessor&);
    ~TunerDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;

private:
    void timerCallback() override;
    void setReference (float hertz);

    BassAmpProcessor& processor;
    juce::TextButton muteButton { "MUTE" }, lowerButton { "-" }, raiseButton { "+" };
    juce::Rectangle<int> titleArea, referenceArea, screenArea;

    float reference = 440.0f;
    float smoothedMidi = 0.0f;
    bool hasPitch = false;
    int framesWithoutPitch = 0;
};
