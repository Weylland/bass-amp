#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "AmpHead.h"
#include "EqPanel.h"
#include "LevelMeters.h"
#include "Pedal.h"
#include "PresetBar.h"
#include "RecorderBar.h"
#include "TunerDisplay.h"

// The whole rig at its design size: header, pre-amp pedalboard, tuner, amp head and
// meters, then the EQ and the post-amp pedalboard. The editor scales it to the window.
class MainPanel : public juce::Component, private juce::Timer
{
public:
    static constexpr int designWidth = 1320;
    static constexpr int designHeight = 800;

    explicit MainPanel (BassAmpProcessor&);
    ~MainPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    static void layoutPedals (juce::Rectangle<int> area, std::initializer_list<Pedal*> pedals);
    static void paintBoard (juce::Graphics&, juce::Rectangle<float>);

    BassAmpProcessor& processor;
    juce::String shownDeviceInfo;

    PresetBar presetBar;
    RecorderBar recorderBar;

    Pedal gate, compressor, octaver, envFilter, fuzz, distortion;
    TunerDisplay tuner;
    AmpHead ampHead;
    LevelMeters meters;
    EqPanel equalizer;
    Pedal phaser, chorus, delay, reverb;

    juce::Rectangle<int> headerArea, preBoard, postBoard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
