#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "AmpHead.h"
#include "EqPanel.h"
#include "LevelMeters.h"
#include "LooperUnit.h"
#include "MetronomeUnit.h"
#include "PedalBoard.h"
#include "PlayerUnit.h"
#include "PresetBar.h"
#include "RecorderBar.h"
#include "TunerDisplay.h"

// The whole rig at its design size: header, pre-amp pedalboard, tuner, amp head and
// meters, the EQ and the post-amp pedalboard, then the practice row (player, looper,
// metronome). The editor scales it to the window.
// Right-clicking a control opens its MIDI learn menu.
class MainPanel : public juce::Component, private juce::Timer
{
public:
    static constexpr int designWidth = 1320;
    static constexpr int designHeight = 896;

    explicit MainPanel (BassAmpProcessor&);
    ~MainPanel() override;

    void paint (juce::Graphics&) override;
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;

private:
    void timerCallback() override;
    void showMidiMenu (const juce::String& paramId);
    juce::Rectangle<int> learningArea() const;

    BassAmpProcessor& processor;
    juce::String shownDeviceInfo;
    juce::String shownLearning;

    PresetBar presetBar;
    RecorderBar recorderBar;

    PedalBoard preBoard, postBoard;
    TunerDisplay tuner;
    AmpHead ampHead;
    LevelMeters meters;
    EqPanel equalizer;
    PlayerUnit player;
    LooperUnit looperUnit;
    MetronomeUnit metronomeUnit;

    juce::Rectangle<int> headerArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
